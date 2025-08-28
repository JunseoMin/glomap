#!/usr/bin/env python3

from __future__ import annotations

import argparse
import cv2
import numpy as np
from pathlib import Path

def extract_and_save_frames(cap, desired_fps: float | None, output_dir: Path):
    fps = cap.get(cv2.CAP_PROP_FPS)
    if desired_fps:
        fps_ratio = desired_fps / fps
    else:
        fps_ratio = 1.0

    output_dir.mkdir(parents=True, exist_ok=True)

    portion = 0.0
    frame_idx = 0
    while True:
        ret, rgb_image = cap.read()
        if not ret:
            break
        portion += fps_ratio
        if portion >= 1.0:
            portion -= 1.0
            cv2.imwrite(str(output_dir / f"{frame_idx:0>5}.jpg"), rgb_image)
            frame_idx += 1


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-o", "--output-dir", type=Path, required=True)
    parser.add_argument("-v", "--video-path", type=Path, required=True)
    parser.add_argument("--desired-fps", type=float, default=60.0)
    args = parser.parse_args()
    frames = extract_frames(cv2.VideoCapture(str(args.video_path)), args.desired_fps)
    save_frames(frames, args.output_dir)

if __name__ == "__main__":
    main()
