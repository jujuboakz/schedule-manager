#ifndef VOICE_RECOGNIZER_H
#define VOICE_RECOGNIZER_H

#include <QObject>
#include <QProcess>
#include <QByteArray>
#include <QString>

extern "C" {
#include "vosk_api.h"
}

class VoiceRecognizer : public QObject
{
    Q_OBJECT
public:
    explicit VoiceRecognizer(QObject *parent = nullptr);
    ~VoiceRecognizer();

    bool init(const QString &modelPath);
    void startRecording(int seconds = 5);   // Â¼ÖÆÃëÊý
    void stopRecording();
    bool isRecording() const;

signals:
    void recognitionResult(const QString &text);
    void statusChanged(const QString &status);

private slots:
    void onRecordingFinished(int exitCode, QProcess::ExitStatus status);
    void onProcessError(QProcess::ProcessError error);

private:
    void cleanup();
    void recognizeWavFile(const QString &filePath);

    VoskModel *m_model;
    VoskRecognizer *m_recognizer;
    QProcess *m_recordingProcess;
    bool m_isRecording;
    bool m_initialized;
    QString m_tempWavFile;
};

#endif // VOICE_RECOGNIZER_H
