#include "recording.h"
#include <colmap/util/misc.h>
// #include <colmap/

namespace glomap {

rerun::RecordingStream rr_rec("GLOMAP");
uint32_t algorithm_step = 0;
std::string image_path_global = "";

void init_recording() {
  rr_rec.spawn().exit_on_failure();
  rr_rec.set_time_sequence("step", algorithm_step);
}

void log_bitmap(rerun::RecordingStream &rec, std::string_view entity_path,  colmap::Bitmap& bitmap) {
  size_t width = bitmap.Width();
  size_t height = bitmap.Height();
  size_t nchannels = bitmap.Channels();
  std::vector<size_t> shape = {height, width, nchannels};
  auto buffer = bitmap.ConvertToRowMajorArray();
  LOG(INFO) << buffer.size();
  LOG(INFO) << entity_path;
  if (nchannels == 3) {
    for (size_t i = 0; i < buffer.size(); i+=3) {
      std::swap(buffer[i], buffer[i+2]);
    }
  }
  rec.log(entity_path, rerun::Image(shape, std::move(buffer)));
}

std::unordered_map<image_t, std::vector<track_t>> get_observation_to_point_map(
  const std::unordered_map<camera_t, Image>& images,
  const std::unordered_map<track_t, Track>& tracks
) {
  // Prepare the 2d-3d correspondences
  std::unordered_map<image_t, std::vector<track_t>> image_to_point3D;
  if (tracks.size()) {
    // Initialize every point to corresponds to invalid point
    for (auto& [image_id, image] : images) {
      if (!image.is_registered)
        continue;
      image_to_point3D[image_id] =
          std::vector<track_t>(image.features.size(), -1);
    }

    for (auto& [track_id, track] : tracks) {
      if (track.observations.size() < 3) {
        continue;
      }
      for (auto& observation : track.observations) {
        if (image_to_point3D.find(observation.first) !=
            image_to_point3D.end()) {
          image_to_point3D[observation.first][observation.second] = track_id;
        }
      }
    }
  }
  return image_to_point3D;
}

void log_reconstruction(
  rerun::RecordingStream &rec,
  const std::unordered_map<camera_t, Camera>& cameras,
  const std::unordered_map<camera_t, Image>& images,
  const std::unordered_map<track_t, Track>& tracks
) {
  std::vector<rerun::Position3D> points;
  std::vector<rerun::Color> colors;

  
  for (auto &[_, image] : images) {
    auto camera = cameras.at(image.camera_id);
    Eigen::Vector3f translation = image.cam_from_world.translation.cast<float>();
    Eigen::Matrix3f rotation = image.cam_from_world.rotation.toRotationMatrix().cast<float>();
    rec.log("images/" + image.file_name, rerun::Transform3D(
      rerun::datatypes::TranslationAndMat3x3(
          rerun::Vec3D(translation.data()),
          rerun::Mat3x3(rotation.data()),
          true
      )
    ));
    rec.log_static("images/" + image.file_name, rerun::ViewCoordinates::RDF);
    Eigen::Matrix3Xf K = camera.GetK().cast<float>();

    rec.log(
      "images/" + image.file_name, 
      rerun::Pinhole(rerun::components::PinholeProjection(rerun::datatypes::Mat3x3(K.data())))
        .with_resolution(int(camera.width), int(camera.height))
    );
  }

  for (auto &[_, track] : tracks) {

    // Should actually be `track.observations.size() < options_.min_num_view_per_track`.
    if (track.observations.size() < 3) continue;
    
    auto xyz = track.xyz;
    points.emplace_back(xyz.x(), xyz.y(), xyz.z());
    colors.emplace_back(track.color[0], track.color[1], track.color[2]);
  }
  rec.log("tracks", rerun::Points3D(points).with_colors(colors));

}

void log_images(rerun::RecordingStream &rec, const std::unordered_map<camera_t, Image>& images, const std::string image_path) {
  for (auto &[id, image] : images) {
    std::string path = colmap::JoinPaths(image_path, image.file_name);
    colmap::Bitmap bitmap;
    if (!bitmap.Read(path)) {
      LOG(ERROR) << "Failed to read image path";
    }
    std::string entity_path = "images/";
    entity_path += image.file_name;
    log_bitmap(rec, entity_path, bitmap);

    std::vector<rerun::Position2D> feature_positions;
    for (auto &point2d : image.features) {
      feature_positions.emplace_back(point2d[0], point2d[1]);
    }
    rec.log(entity_path + "/features", rerun::Points2D(feature_positions));
  }
}

}