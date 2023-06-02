import io
import cv2
import numpy as np
import pyrgbd as rgbd


def convert_yuv420_to_rgb(y_array: np.ndarray, u_array: np.ndarray, v_array: np.ndarray
) -> np.ndarray:
    # Open In-memory bytes streams (instead of using fifo)
    f = io.BytesIO()

    # Write Y, U and V to the "streams".
    f.write(y_array.tobytes())
    f.write(u_array.tobytes())
    f.write(v_array.tobytes())

    f.seek(0)

    data = f.read(y_array.size * 3 // 2)
    # Reshape data to numpy array with height*1.5 rows
    yuv_data = np.frombuffer(data, np.uint8).reshape(
        y_array.shape[0] * 3 // 2, y_array.shape[1]
    )

    # Convert YUV to RGB
    return cv2.cvtColor(yuv_data, cv2.COLOR_YUV2RGB_I420)


def convert_yuv_frame_to_rgb_array(yuv_frame: rgbd.YuvFrame) -> np.ndarray:
    return convert_yuv420_to_rgb(
        yuv_frame.get_y_channel(), yuv_frame.get_u_channel(), yuv_frame.get_v_channel()
    )


def convert_rgb_to_yuv420(rgb_array: np.ndarray):
    rgb_width = rgb_array.shape[1]
    rgb_height = rgb_array.shape[0]
    yuv_array = cv2.cvtColor(rgb_array, cv2.COLOR_RGB2YUV_I420)

    y_array = yuv_array[0:rgb_height, :]
    u_array = yuv_array[rgb_height : rgb_height * 5 // 4, :]
    v_array = yuv_array[rgb_height * 5 // 4 : rgb_height * 6 // 4, :]

    u_array = np.reshape(u_array, (rgb_height // 2, rgb_width // 2))
    v_array = np.reshape(v_array, (rgb_height // 2, rgb_width // 2))

    return y_array, u_array, v_array


def convert_rgb_array_to_yuv_frame(rgb_array: np.ndarray) -> np.ndarray:
    y_array, u_array, v_array = convert_rgb_to_yuv420(rgb_array)
    return rgbd.YuvFrame(y_array, u_array, v_array)
