#include "VoiceRecognizer.h"
#include <QDebug>
#include <QFile>
#include <QDir>

VoiceRecognizer::VoiceRecognizer(QObject *parent)
    : QObject(parent)
    , m_model(nullptr)
    , m_recognizer(nullptr)
    , m_recordingProcess(nullptr)
    , m_isRecording(false)
    , m_initialized(false)
    , m_stoppedByUser(false)           // 新增：初始化标志
    , m_tempWavFile("/tmp/voice_temp.wav")
{
}

VoiceRecognizer::~VoiceRecognizer()
{
    stopRecording();
    cleanup();
}

bool VoiceRecognizer::init(const QString &modelPath)
{
    if (m_initialized) return true;

    vosk_set_log_level(0);

    m_model = vosk_model_new(modelPath.toUtf8().constData());
    if (!m_model) {
        qDebug() << "Vosk 模型加载失败:" << modelPath;
        return false;
    }

    m_recognizer = vosk_recognizer_new(m_model, 16000.0);
    if (!m_recognizer) {
        qDebug() << "Vosk 识别器创建失败";
        vosk_model_free(m_model);
        m_model = nullptr;
        return false;
    }

    m_initialized = true;
    qDebug() << "Vosk 初始化成功";
    emit statusChanged("语音识别已就绪");
    return true;
}

void VoiceRecognizer::startRecording(int seconds)
{
    if (!m_initialized) {
        emit statusChanged("错误：请先初始化语音识别");
        return;
    }

    if (m_isRecording) {
        qDebug() << "已经在录音中";
        return;
    }

    // ===== 新增：重置标志位 =====
    m_stoppedByUser = false;

    // 删除旧的临时文件
    QFile::remove(m_tempWavFile);

    // 创建 QProcess
    m_recordingProcess = new QProcess(this);
    connect(m_recordingProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &VoiceRecognizer::onRecordingFinished);
    connect(m_recordingProcess, &QProcess::errorOccurred,
            this, &VoiceRecognizer::onProcessError);

    // 构建命令：arecord -d 秒数 -f S16_LE -r 16000 -c 1 -t wav 输出文件
    QStringList args;
    args << "-d" << QString::number(seconds)
         << "-f" << "S16_LE"
         << "-r" << "16000"
         << "-c" << "1"
         << "-t" << "wav"
         << m_tempWavFile;

    qDebug() << "执行命令: arecord" << args.join(" ");

    m_recordingProcess->start("arecord", args);
    if (!m_recordingProcess->waitForStarted(2000)) {
        qDebug() << "启动 arecord 失败";
        emit statusChanged("错误：无法启动录音 (arecord 未安装)");
        delete m_recordingProcess;
        m_recordingProcess = nullptr;
        return;
    }

    m_isRecording = true;
    qDebug() << "开始录音，时长:" << seconds << "秒";
    emit statusChanged(QString("🎤 录音中... (%1秒)").arg(seconds));
}

void VoiceRecognizer::stopRecording()
{
    if (!m_isRecording || !m_recordingProcess) return;

    // ===== 新增：标记为用户手动停止 =====
    m_stoppedByUser = true;

    // 断开信号连接，防止 onRecordingFinished 被触发导致重复删除
    disconnect(m_recordingProcess, nullptr, this, nullptr);

    m_recordingProcess->terminate();
    m_recordingProcess->waitForFinished(1000);
    if (m_recordingProcess->state() == QProcess::Running) {
        m_recordingProcess->kill();
        m_recordingProcess->waitForFinished(500);
    }

    // 手动清理进程
    delete m_recordingProcess;
    m_recordingProcess = nullptr;
    m_isRecording = false;

    emit statusChanged("录音已手动停止");
    qDebug() << "用户手动停止录音";
}

bool VoiceRecognizer::isRecording() const
{
    return m_isRecording;
}

void VoiceRecognizer::onRecordingFinished(int exitCode, QProcess::ExitStatus status)
{
    // ===== 新增：如果是用户手动停止，跳过识别逻辑 =====
    if (m_stoppedByUser) {
        qDebug() << "用户手动停止，跳过识别";
        // 清理进程（如果还没被清理）
        if (m_recordingProcess) {
            delete m_recordingProcess;
            m_recordingProcess = nullptr;
        }
        m_isRecording = false;
        return;
    }

    m_isRecording = false;

    // 清理进程
    if (m_recordingProcess) {
        delete m_recordingProcess;
        m_recordingProcess = nullptr;
    }

    if (status == QProcess::CrashExit || exitCode != 0) {
        qDebug() << "录音进程异常退出，exitCode:" << exitCode;
        emit statusChanged("录音失败");
        return;
    }

    qDebug() << "录音完成，文件:" << m_tempWavFile;
    emit statusChanged("⏳ 正在识别...");

    // 识别音频文件
    recognizeWavFile(m_tempWavFile);
}

void VoiceRecognizer::onProcessError(QProcess::ProcessError error)
{
    qDebug() << "录音进程错误:" << error;

    // ===== 新增：如果已经标记为手动停止，不处理 =====
    if (m_stoppedByUser) {
        qDebug() << "用户已手动停止，忽略进程错误";
        return;
    }

    m_isRecording = false;
    if (m_recordingProcess) {
        delete m_recordingProcess;
        m_recordingProcess = nullptr;
    }
    emit statusChanged("录音进程出错");
}

void VoiceRecognizer::recognizeWavFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开录音文件:" << filePath;
        emit statusChanged("无法读取录音文件");
        return;
    }

    QByteArray audioData = file.readAll();
    file.close();

    if (audioData.isEmpty()) {
        qDebug() << "录音文件为空";
        emit statusChanged("录音数据为空");
        return;
    }

    // 跳过 WAV 头部（44 字节），将 PCM 数据送入 Vosk
    const char *pcmData = audioData.constData() + 44;
    int pcmSize = audioData.size() - 44;

    if (pcmSize <= 0) {
        qDebug() << "PCM 数据为空";
        emit statusChanged("录音数据格式错误");
        return;
    }

    if (vosk_recognizer_accept_waveform(m_recognizer, pcmData, pcmSize)) {
        QString result = QString::fromUtf8(vosk_recognizer_result(m_recognizer));
        qDebug() << "识别结果:" << result;
        emit recognitionResult(result);
        emit statusChanged("识别完成");
    } else {
        QString partial = QString::fromUtf8(vosk_recognizer_partial_result(m_recognizer));
        if (!partial.isEmpty() && partial != "{}") {
            emit recognitionResult(partial);
            emit statusChanged("部分识别结果");
        } else {
            qDebug() << "未识别到有效语音";
            emit statusChanged("未识别到语音，请重试");
        }
    }

    // 删除临时文件
    QFile::remove(filePath);
}

void VoiceRecognizer::cleanup()
{
    // ===== 修改：清理时也检查进程 =====
    if (m_recordingProcess) {
        disconnect(m_recordingProcess, nullptr, this, nullptr);
        m_recordingProcess->terminate();
        m_recordingProcess->waitForFinished(500);
        delete m_recordingProcess;
        m_recordingProcess = nullptr;
    }

    if (m_recognizer) {
        vosk_recognizer_free(m_recognizer);
        m_recognizer = nullptr;
    }
    if (m_model) {
        vosk_model_free(m_model);
        m_model = nullptr;
    }
    m_initialized = false;
    m_isRecording = false;
    m_stoppedByUser = false;
}
