import numpy as np
from ._librgbd_ffi import lib
from .capi_containers import NativeByteArray

class NativeAudioEncoderFrame:
    def __init__(self, ptr):
        self.ptr = ptr

    def close(self):
        lib.rgbd_audio_encoder_frame_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def get_packet_bytes_list_count(self):
        return lib.rgbd_audio_encoder_frame_get_packet_bytes_list_count(self.ptr)

    def get_packet_bytes(self, index: int) -> np.ndarray:
        return NativeByteArray(lib.rgbd_audio_encoder_frame_get_packet_bytes(self.ptr, index)).to_np_array()


class NativeAudioEncoder:
    def __init__(self):
        self.ptr = lib.rgbd_audio_encoder_ctor()

    def close(self):
        lib.rgbd_audio_encoder_dtor(self.ptr)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def encode(self, pcm_samples_data, pcm_samples_size) -> NativeAudioEncoderFrame:
        return NativeAudioEncoderFrame(lib.rgbd_audio_encoder_encode(self.ptr, pcm_samples_data, pcm_samples_size))

    def flush(self) -> NativeAudioEncoderFrame:
        return NativeAudioEncoderFrame(lib.rgbd_audio_encoder_flush(self.ptr))
