#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
#include "third_party/openvslam/feature/orb_extractor.h"
#include "third_party/openvslam/util/guided_matching.h"
#include "slam_datastructures/frame.h"
#include "slam_datastructures/keyframe.h"
#include "slam_datastructures/landmark.h"
#include "slam_datastructures/camera.h"
#include "slam_datastructures/local_map_cleaner.h"
#include "slam_debug.h"
#include "types.h"
namespace py = pybind11;


PYBIND11_MODULE(cslam, m) {

py::class_<openvslam::feature::orb_extractor>(m, "orb_extractor")
    .def(py::init<const unsigned int, const float, const unsigned int,
                  const unsigned int, const unsigned int>())
    .def("extract_orb_py", &openvslam::feature::orb_extractor::extract_orb_py)
    .def("extract_orb_py2", &openvslam::feature::orb_extractor::extract_orb_py2)
    .def("get_scale_factors", &openvslam::feature::orb_extractor::get_scale_factors);


// Helper class
py::class_<cslam::GridParameters>(m, "GridParameters")
    .def(py::init<unsigned int, unsigned int, float, float, float, float, float, float>());


// Matcher
py::class_<cslam::GuidedMatcher>(m, "GuidedMatcher")
    .def(py::init<const cslam::GridParameters&, const cslam::BrownPerspectiveCamera&>())
    .def("assign_keypoints_to_grid", &cslam::GuidedMatcher::assign_keypoints_to_grid)
    .def("distribute_keypoints_to_grid_frame", &cslam::GuidedMatcher::distribute_keypoints_to_grid_frame)
    .def("match_frame_to_frame", &cslam::GuidedMatcher::match_frame_to_frame)
    .def("match_frame_and_landmarks", &cslam::GuidedMatcher::match_frame_and_landmarks)
    .def("match_current_and_last_frame", &cslam::GuidedMatcher::match_current_and_last_frame)
    .def("match_for_triangulation", &cslam::GuidedMatcher::match_for_triangulation)
    .def("update_local_landmarks", &cslam::GuidedMatcher::update_local_landmarks)
    .def("search_local_landmarks", &cslam::GuidedMatcher::search_local_landmarks)
    .def("create_E_21", &cslam::GuidedMatcher::create_E_21);

// Frame
py::class_<cslam::Frame>(m, "Frame")
        .def(py::init<const csfm::pyarray_uint8, const csfm::pyarray_uint8, const std::string&,
                      const size_t, openvslam::feature::orb_extractor*>())
        .def_readonly("frame_id", &cslam::Frame::frame_id)
        .def_readonly("im_name", &cslam::Frame::im_name)
        .def_readwrite("parent_kf", &cslam::Frame::mParentKf)
        .def("getKptsAndDescPy",&cslam::Frame::getKptsAndDescPy)
        .def("getKptsPy",&cslam::Frame::getKptsPy)
        .def("getDescPy",&cslam::Frame::getDescPy)
        .def("getKptsUndist", &cslam::Frame::getKptsUndist)
        .def("add_landmark", &cslam::Frame::add_landmark)
        .def("set_pose", &cslam::Frame::setPose)
        .def("get_pose", &cslam::Frame::getPose)
        .def("get_valid_lms", &cslam::Frame::get_valid_lms)
        .def("get_valid_keypts", &cslam::Frame::get_valid_keypts)
        .def("set_outlier", &cslam::Frame::set_outlier)
        .def("get_valid_idx", &cslam::Frame::get_valid_idx)
        .def("discard_outliers", &cslam::Frame::discard_outliers)
        .def("clean_and_tick_landmarks", &cslam::Frame::clean_and_tick_landmarks);
        
// Keyframe
py::class_<cslam::KeyFrame>(m, "KeyFrame")
    .def(py::init<const size_t, const cslam::Frame>())
    .def_readonly("kf_id", &cslam::KeyFrame::kf_id_)
    .def_readonly("im_name", &cslam::KeyFrame::im_name_)
    .def("set_pose", &cslam::KeyFrame::setPose)
    .def("get_pose", &cslam::KeyFrame::getPose)
    .def("add_landmark", &cslam::KeyFrame::add_landmark)
    .def("get_num_tracked_lms", &cslam::KeyFrame::get_num_tracked_lms)
    .def("get_cam_center", &cslam::KeyFrame::get_cam_center)
    .def("compute_median_depth", &cslam::KeyFrame::compute_median_depth)
    .def("getKptsPy",&cslam::KeyFrame::getKptsPy)
    .def("getDescPy",&cslam::KeyFrame::getDescPy)
    .def("getKptsUndist", &cslam::KeyFrame::getKptsUndist)
    .def("compute_local_keyframes", &cslam::KeyFrame::compute_local_keyframes);
    // .def("update_lms_after_kf_insert", &cslam::KeyFrame::update_lms_after_kf_insert);

// Landmark
py::class_<cslam::Landmark>(m, "Landmark")
    .def(py::init<const size_t, cslam::KeyFrame*, const Eigen::Vector3f&>())
    .def_readonly("lm_id", &cslam::Landmark::lm_id_)
    .def("increase_num_observable", &cslam::Landmark::increase_num_observable)
    .def("increase_num_observed", &cslam::Landmark::increase_num_observed)
    .def("get_observed_ratio", &cslam::Landmark::get_observed_ratio)
    .def("add_observation", &cslam::Landmark::add_observation)
    .def("update_normal_and_depth", &cslam::Landmark::update_normal_and_depth)
    .def("compute_descriptor", &cslam::Landmark::compute_descriptor)
    .def("get_pos_in_world", &cslam::Landmark::get_pos_in_world)
    .def("is_observable_in_kf", &cslam::Landmark::is_observed_in_keyframe);


//Camera
py::class_<cslam::BrownPerspectiveCamera>(m, "BrownPerspectiveCamera")
    .def(py::init<const size_t, const size_t, const std::string&,
                    const float, const float, const float, const float,
                    const float, const float, const float, const float, const float>())
    .def("undistKeyptsFrame", &cslam::BrownPerspectiveCamera::undistKeyptsFrame)
    .def("convertKeyptsToBearingsFrame", &cslam::BrownPerspectiveCamera::convertKeyptsToBearingsFrame);

//LocalMapCleanr
py::class_<cslam::LocalMapCleaner>(m, "LocalMapCleaner")
    .def(py::init<const cslam::GuidedMatcher&>()) //, cslam::BrownPerspectiveCamera*>())
    .def("update_lms_after_kf_insert", &cslam::LocalMapCleaner::update_lms_after_kf_insert)
    .def("remove_redundant_lms", &cslam::LocalMapCleaner::remove_redundant_landmarks)
    .def("add_landmark", &cslam::LocalMapCleaner::add_landmark)
    .def("fuse_landmark_duplication", &cslam::LocalMapCleaner::fuse_landmark_duplication)
    .def("update_new_keyframe", &cslam::LocalMapCleaner::update_new_keyframe);


py::class_<cslam::SlamDebug>(m, "SlamDebug")
    .def(py::init<>())
    .def("print_matches_from_lms", &cslam::SlamDebug::print_matches_from_lms);
}
