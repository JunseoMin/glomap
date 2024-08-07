#!/usr/bin/env bash


colmap feature_extractor \
    --image_path    $1/images \
    --database_path $1/database.db

# Use sequential_matcher since images are from a video.
colmap sequential_matcher \
    --database_path $1/database.db 

# glomap mapper \
#     --database_path $1/database.db \
#     --image_path    $1/images \
#     --output_path   ./output/$1/sparse

