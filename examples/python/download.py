import argparse
import base64
import requests


def decode_base64url_to_long(s: str):
    return int.from_bytes(base64.urlsafe_b64decode(s + "==="), "big")


def main():
    parser = argparse.ArgumentParser(
        prog="pyrgbd example imshow",
        description="pyrgbd example of showing video frames using OpenCV.",
    )
    parser.add_argument("-i", "--input", help="URL of the Telegie post to download.")
    parser.add_argument("-o", "--output", help="Path to save the downloaded 3D file.")

    args = parser.parse_args()
    if args.input is None:
        print(
            "No Telegie post URL specified."
        )
        return
    if args.output is None:
        print(
            "No path to save the 3D video file specified."
        )
        return

    telegie_post_url = args.input
    if telegie_post_url.endswith("\\"):
        telegie_post_url = telegie_post_url[0:-1]
    base64url_post_id = telegie_post_url.split("/")[-1]
    print(f"base64url_post_id: {base64url_post_id}")
    post_id = decode_base64url_to_long(base64url_post_id)
    telegie_file_url = (
        f"https://posts.telegie.com/v1/{post_id}/{post_id}.mkv"
    )
    print(f"telegie_file_url: {telegie_file_url}")

    response = requests.get(telegie_file_url)
    with open(args.output, "wb+") as record:
        record.write(response.content)


if __name__ == "__main__":
    main()
