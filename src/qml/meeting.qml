import QMediaCaptureSession
import QCamera
import QAudioInput
import QMediaRecorder

import QQuick



CaptureSession {
    id: captureSession
    camera: Camera { id: camera }
    audioInput: AudioInput { id: audioInput }
    recorder: MediaRecorder { id: recorder }
    videoOutput: VideoOutput { id: preview }
}

Button {
    text: "Record"
    onClicked: {
        camera.start()
        recorder.record()
    }
}