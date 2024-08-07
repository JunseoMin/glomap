# GLOMAP: Global Structure-from-Motion Revisited

[Project page](https://lpanaf.github.io/eccv24_glomap/) | [Paper](https://arxiv.org/pdf/2407.20219)
---

## About

GLOMAP is a general purpose global structure-from-motion pipeline for
image-based reconstruction. GLOMAP requires a COLMAP database as input and
outputs a COLMAP sparse reconstruction. As compared to COLMAP, this project
provides a much more efficient and scalable reconstruction process, typically
1-2 orders of magnitude faster, with on-par or superior reconstruction quality.

If you use this project for your research, please cite
```
@inproceedings{pan2024glomap,
    author={Pan, Linfei and Barath, Daniel and Pollefeys, Marc and Sch\"{o}nberger, Johannes Lutz},
    title={{Global Structure-from-Motion Revisited}},
    booktitle={European Conference on Computer Vision (ECCV)},
    year={2024},
}
```


## End-to-End with Docker.

Download one of the [official datasets](https://colmap.github.io/datasets.html) unzip it.
For example if we download the `south-building.zip`
```bash
mkdir -p data
unzip south-building.zip -d data/
```

Build and enter the docker container by running the following scripts:
```bash
cd docker
./build.sh # Builds the docker image
./start.sh # Starts the docker container
./attach.sh # Enter the container

```

When we're inside the container we can build `glomap` and install it.
```bash
mkdir -p build
cd build
cmake .. -GNinja
ninja
ninja install
```

To run it we must first extract the features and match them with `colmap`.
```bash
cd /ws
colmap feature_extractor \
    --image_path    data/south-building/images \
    --database_path data/south-building/database.db

colmap sequential_matcher \
    --database_path data/south-building/database.db 
```

Now we're ready to run and visualize `glomap`:
```bash
glomap mapper \
    --database_path data/south-building/database.db \
    --image_path    data/south-building/images \
    --output_path   data/south-building/sparse
```
To cleary see what's happening it's recommended you open the `blueprint.rbl` file in this directory.
If you want a cooler visualization you can resume a mapping, you do this by first running the steps above and then running the following command:
```bash
glomap mapper_resume \
    --image_path data/south-building/images \
    --input_path data/south-building/sparse/0 \
    --output_path data/south-building/sparse
```

If you're experiencing performance issues with rerun you might have to run the viewer outside of the docker container.

### From video
To run `glomap` on a video you will first have to split it into multiple frames and put them in a directory. To easily do this you can use the the script in `scripts/extract_frames.py` like this:
```bash
# Split 'colosseum.mp4' into frames at 1 frame per second and puts the images in colosseum/images.
scripts/extract_frames.py -v colosseum.mp4 -o colosseum/images --desired-fps 1

# Then you can extract the features like normally.
colmap feature_extractor \
    --image_path    colosseum/images \
    --database_path colosseum/database.db
  colmap sequential_matcher \ 
    --database_path colosseum/database.db 

glomap mapper \
    --database_path colosseum/database.db \
    --image_path    colosseum/images \
    --output_path   colosseum/sparse
```

### Notes

- For larger scale datasets, it is recommended to use `sequential_matcher` or
  `vocab_tree_matcher` from `COLMAP`.
```shell
colmap sequential_matcher --database_path DATABASE_PATH
colmap vocab_tree_matcher --database_path DATABASE_PATH --VocabTreeMatching.vocab_tree_path VOCAB_TREE_PATH
```
- Alternatively, one can use
  [hloc](https://github.com/cvg/Hierarchical-Localization/) for image retrieval
  and matching with learning-based descriptors.



## Acknowledgement

We are highly inspired by COLMAP, PoseLib, Theia. Please consider also citing
them, if using GLOMAP in your work.

## Support

Please, use GitHub Discussions at https://github.com/colmap/glomap/discussions
for questions and the GitHub issue tracker at https://github.com/colmap/glomap
for bug reports, feature requests/additions, etc.

## Contribution

Contributions (bug reports, bug fixes, improvements, etc.) are very welcome and
should be submitted in the form of new issues and/or pull requests on GitHub.

## License

```
Copyright (c) 2024, ETH Zurich.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of ETH Zurich nor the names of its contributors may
      be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
```
