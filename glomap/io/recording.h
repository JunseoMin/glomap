#pragma once
#include <rerun.hpp>
#include "colmap/sensor/bitmap.h"
#include <colmap/scene/reconstruction.h>
#include "glomap/estimators/global_positioning.h"

namespace glomap {

extern rerun::RecordingStream rr_rec;
extern uint32_t algorithm_step;
extern std::string image_path_global; // A hack to get it into the LogginCallback class without passing the option everywhere.

void log_reconstruction(
  rerun::RecordingStream &rec,
  const std::unordered_map<camera_t, Camera>& cameras,
  const std::unordered_map<camera_t, Image>& images,
  const std::unordered_map<track_t, Track>& tracks
);

void log_colmap_reconstruction(rerun::RecordingStream& rec, colmap::Reconstruction& reconstruction);
void log_bitmap(rerun::RecordingStream &rec, std::string_view entity_path,  colmap::Bitmap& bitmap);
void log_images(rerun::RecordingStream &rec, const std::unordered_map<camera_t, Image>& images, const std::string image_path);
void init_recording();

}
