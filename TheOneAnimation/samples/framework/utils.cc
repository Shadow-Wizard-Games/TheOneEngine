//----------------------------------------------------------------------------//
//                                                                            //
// ozz-animation is hosted at http://github.com/guillaumeblanc/ozz-animation  //
// and distributed under the MIT License (MIT).                               //
//                                                                            //
// Copyright (c) Guillaume Blanc                                              //
//                                                                            //
// Permission is hereby granted, free of charge, to any person obtaining a    //
// copy of this software and associated documentation files (the "Software"), //
// to deal in the Software without restriction, including without limitation  //
// the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
// and/or sell copies of the Software, and to permit persons to whom the      //
// Software is furnished to do so, subject to the following conditions:       //
//                                                                            //
// The above copyright notice and this permission notice shall be included in //
// all copies or substantial portions of the Software.                        //
//                                                                            //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    //
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    //
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        //
// DEALINGS IN THE SOFTWARE.                                                  //
//                                                                            //
//----------------------------------------------------------------------------//

#include "utils.h"

#include <cassert>
#include <limits>

#include "mesh.h"
#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/track.h"
#include "ozz/base/io/archive.h"
#include "ozz/base/io/stream.h"
#include "ozz/base/log.h"
#include "ozz/base/maths/box.h"
#include "ozz/base/maths/simd_math.h"
#include "ozz/base/maths/simd_quaternion.h"
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/memory/allocator.h"
#include "ozz/geometry/runtime/skinning_job.h"

namespace ozz {
    namespace sample {

        PlaybackController::PlaybackController()
            : time_ratio_(0.f),
            previous_time_ratio_(0.f),
            playback_speed_(1.f),
            play_(true),
            loop_(true) {}

        void PlaybackController::Update(const animation::Animation& _animation,
            float _dt) {
            float new_time = time_ratio_;

            if (play_) {
                new_time = time_ratio_ + _dt * playback_speed_ / _animation.duration();
            }

            // Must be called even if time doesn't change, in order to update previous
            // frame time ratio. Uses set_time_ratio function in order to update
            // previous_time_ an wrap time value in the unit interval (depending on loop
            // mode).
            set_time_ratio(new_time);
        }

        void PlaybackController::set_time_ratio(float _ratio) {
            previous_time_ratio_ = time_ratio_;
            if (loop_) {
                // Wraps in the unit interval [0:1], even for negative values (the reason
                // for using floorf).
                time_ratio_ = _ratio - floorf(_ratio);
            }
            else {
                // Clamps in the unit interval [0:1].
                time_ratio_ = math::Clamp(0.f, _ratio, 1.f);
            }
            update_events();
        }

        // Gets animation current time.
        float PlaybackController::time_ratio() const { return time_ratio_; }

        // Gets animation time of last update.
        float PlaybackController::previous_time_ratio() const {
            return previous_time_ratio_;
        }

        void PlaybackController::update_events()
        {
            size_t keys_size = key_events_.size();

            for (size_t i = 0; i < keys_size; i++) {
                AnimKeyEvent& ake = key_events_[i];

                float diff = time_ratio_ - previous_time_ratio_;

                if (time_ratio_ >= ake.time) {
                    //si
                    if (previous_time_ratio_ < ake.time) {
                        ake.action();
                    }
                }
                else if (diff < 0) {
                    if (previous_time_ratio_ < ake.time) {
                        ake.action();
                    }
                }
            }
        }

        void PlaybackController::add_key_event(Action<> action, float time)
        {
            key_events_.push_back({ action, time });
        }

        void PlaybackController::remove_key_event(Action<> action)
        {
            size_t keys_size = key_events_.size();

            for (size_t i = 0; i < keys_size; i++) {
                if (key_events_[i].action == action) {
                    key_events_.erase(key_events_.begin() + i);
                    break;
                }
            }
        }

        void PlaybackController::Reset() {
            previous_time_ratio_ = time_ratio_ = 0.f;
            playback_speed_ = 1.f;
            play_ = true;
        }

        // Uses LocalToModelJob to compute skeleton model space posture, then forwards
        // to ComputePostureBounds
        void ComputeSkeletonBounds(const animation::Skeleton& _skeleton,
            math::Box* _bound) {
            using ozz::math::Float4x4;

            assert(_bound);

            // Set a default box.
            *_bound = ozz::math::Box();

            const int num_joints = _skeleton.num_joints();
            if (!num_joints) {
                return;
            }

            // Allocate matrix array, out of memory is handled by the LocalToModelJob.
            ozz::vector<ozz::math::Float4x4> models(num_joints);

            // Compute model space rest pose.
            ozz::animation::LocalToModelJob job;
            job.input = _skeleton.joint_rest_poses();
            job.output = make_span(models);
            job.skeleton = &_skeleton;
            if (job.Run()) {
                // Forwards to posture function.
                ComputePostureBounds(job.output, _bound);
            }
        }

        // Loop through matrices and collect min and max bounds.
        void ComputePostureBounds(ozz::span<const ozz::math::Float4x4> _matrices,
            math::Box* _bound) {
            assert(_bound);

            // Set a default box.
            *_bound = ozz::math::Box();

            if (_matrices.empty()) {
                return;
            }

            // Loops through matrices and stores min/max.
            // Matrices array cannot be empty, it was checked at the beginning of the
            // function.
            const ozz::math::Float4x4* current = _matrices.begin();
            math::SimdFloat4 min = current->cols[3];
            math::SimdFloat4 max = current->cols[3];
            ++current;
            while (current < _matrices.end()) {
                min = math::Min(min, current->cols[3]);
                max = math::Max(max, current->cols[3]);
                ++current;
            }

            // Stores in math::Box structure.
            math::Store3PtrU(min, &_bound->min.x);
            math::Store3PtrU(max, &_bound->max.x);

            return;
        }

        void MultiplySoATransformQuaternion(
            int _index, const ozz::math::SimdQuaternion& _quat,
            const ozz::span<ozz::math::SoaTransform>& _transforms) {
            assert(_index >= 0 && static_cast<size_t>(_index) < _transforms.size() * 4 &&
                "joint index out of bound.");

            // Convert soa to aos in order to perform quaternion multiplication, and gets
            // back to soa.
            ozz::math::SoaTransform& soa_transform_ref = _transforms[_index / 4];
            ozz::math::SimdQuaternion aos_quats[4];
            ozz::math::Transpose4x4(&soa_transform_ref.rotation.x, &aos_quats->xyzw);

            ozz::math::SimdQuaternion& aos_quat_ref = aos_quats[_index & 3];
            aos_quat_ref = aos_quat_ref * _quat;

            ozz::math::Transpose4x4(&aos_quats->xyzw, &soa_transform_ref.rotation.x);
        }

        bool LoadSkeleton(const char* _filename, ozz::animation::Skeleton* _skeleton) {
            assert(_filename && _skeleton);
            ozz::log::Out() << "Loading skeleton archive " << _filename << "."
                << std::endl;
            ozz::io::File file(_filename, "rb");
            if (!file.opened()) {
                ozz::log::Err() << "Failed to open skeleton file " << _filename << "."
                    << std::endl;
                return false;
            }
            ozz::io::IArchive archive(&file);
            if (!archive.TestTag<ozz::animation::Skeleton>()) {
                ozz::log::Err() << "Failed to load skeleton instance from file "
                    << _filename << "." << std::endl;
                return false;
            }

            // Once the tag is validated, reading cannot fail.
            archive >> *_skeleton;

            return true;
        }

        bool LoadAnimation(const char* _filename,
            ozz::animation::Animation* _animation) {
            assert(_filename && _animation);
            ozz::log::Out() << "Loading animation archive: " << _filename << "."
                << std::endl;
            ozz::io::File file(_filename, "rb");
            if (!file.opened()) {
                ozz::log::Err() << "Failed to open animation file " << _filename << "."
                    << std::endl;
                return false;
            }
            ozz::io::IArchive archive(&file);
            if (!archive.TestTag<ozz::animation::Animation>()) {
                ozz::log::Err() << "Failed to load animation instance from file "
                    << _filename << "." << std::endl;
                return false;
            }

            // Once the tag is validated, reading cannot fail.
            archive >> *_animation;

            return true;
        }

        namespace {
            template <typename _Track>
            bool LoadTrackImpl(const char* _filename, _Track* _track) {
                assert(_filename && _track);
                ozz::log::Out() << "Loading track archive: " << _filename << "." << std::endl;
                ozz::io::File file(_filename, "rb");
                if (!file.opened()) {
                    ozz::log::Err() << "Failed to open track file " << _filename << "."
                        << std::endl;
                    return false;
                }
                ozz::io::IArchive archive(&file);
                if (!archive.TestTag<_Track>()) {
                    ozz::log::Err() << "Failed to load float track instance from file "
                        << _filename << "." << std::endl;
                    return false;
                }

                // Once the tag is validated, reading cannot fail.
                archive >> *_track;

                return true;
            }
        }  // namespace

        bool LoadTrack(const char* _filename, ozz::animation::FloatTrack* _track) {
            return LoadTrackImpl(_filename, _track);
        }
        bool LoadTrack(const char* _filename, ozz::animation::Float2Track* _track) {
            return LoadTrackImpl(_filename, _track);
        }
        bool LoadTrack(const char* _filename, ozz::animation::Float3Track* _track) {
            return LoadTrackImpl(_filename, _track);
        }
        bool LoadTrack(const char* _filename, ozz::animation::Float4Track* _track) {
            return LoadTrackImpl(_filename, _track);
        }
        bool LoadTrack(const char* _filename, ozz::animation::QuaternionTrack* _track) {
            return LoadTrackImpl(_filename, _track);
        }

        bool LoadMesh(const char* _filename, ozz::sample::Mesh* _mesh) {
            assert(_filename && _mesh);
            ozz::log::Out() << "Loading mesh archive: " << _filename << "." << std::endl;
            ozz::io::File file(_filename, "rb");
            if (!file.opened()) {
                ozz::log::Err() << "Failed to open mesh file " << _filename << "."
                    << std::endl;
                return false;
            }
            ozz::io::IArchive archive(&file);
            if (!archive.TestTag<ozz::sample::Mesh>()) {
                ozz::log::Err() << "Failed to load mesh instance from file " << _filename
                    << "." << std::endl;
                return false;
            }

            // Once the tag is validated, reading cannot fail.
            archive >> *_mesh;

            return true;
        }

        bool LoadMeshes(const char* _filename,
            ozz::vector<ozz::sample::Mesh>* _meshes) {
            assert(_filename && _meshes);
            ozz::log::Out() << "Loading meshes archive: " << _filename << "."
                << std::endl;
            ozz::io::File file(_filename, "rb");
            if (!file.opened()) {
                ozz::log::Err() << "Failed to open mesh file " << _filename << "."
                    << std::endl;
                return false;
            }
            ozz::io::IArchive archive(&file);

            while (archive.TestTag<ozz::sample::Mesh>()) {
                _meshes->resize(_meshes->size() + 1);
                archive >> _meshes->back();
            }

            return true;
        }

        namespace {
            // Moller–Trumbore intersection algorithm
            // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
            bool RayIntersectsTriangle(const ozz::math::Float3& _ray_origin,
                const ozz::math::Float3& _ray_direction,
                const ozz::math::Float3& _p0,
                const ozz::math::Float3& _p1,
                const ozz::math::Float3& _p2,
                ozz::math::Float3* _intersect,
                ozz::math::Float3* _normal) {
                const float kEpsilon = 0.0000001f;

                const ozz::math::Float3 edge1 = _p1 - _p0;
                const ozz::math::Float3 edge2 = _p2 - _p0;
                const ozz::math::Float3 h = Cross(_ray_direction, edge2);

                const float a = Dot(edge1, h);
                if (a > -kEpsilon && a < kEpsilon) {
                    return false;  // This ray is parallel to this triangle.
                }

                const float inv_a = 1.f / a;
                const ozz::math::Float3 s = _ray_origin - _p0;
                const float u = Dot(s, h) * inv_a;
                if (u < 0.f || u > 1.f) {
                    return false;
                }

                const ozz::math::Float3 q = Cross(s, edge1);
                const float v = ozz::math::Dot(_ray_direction, q) * inv_a;
                if (v < 0.f || u + v > 1.f) {
                    return false;
                }

                // At this stage we can compute t to find out where the intersection point is
                // on the line.
                const float t = Dot(edge2, q) * inv_a;

                if (t > kEpsilon) {  // Ray intersection
                    *_intersect = _ray_origin + _ray_direction * t;
                    *_normal = Normalize(Cross(edge1, edge2));
                    return true;
                }
                else {  // This means that there is a line intersection but not a ray
                    // intersection.
                    return false;
                }
            }
        }  // namespace

        bool RayIntersectsMesh(const ozz::math::Float3& _ray_origin,
            const ozz::math::Float3& _ray_direction,
            const ozz::sample::Mesh& _mesh,
            ozz::math::Float3* _intersect,
            ozz::math::Float3* _normal) {
            assert(_mesh.parts.size() == 1 && !_mesh.skinned());

            bool intersected = false;
            ozz::math::Float3 intersect, normal;
            const float* vertices = array_begin(_mesh.parts[0].positions);
            const uint16_t* indices = array_begin(_mesh.triangle_indices);
            for (int i = 0; i < _mesh.triangle_index_count(); i += 3) {
                const float* pf0 = vertices + indices[i + 0] * 3;
                const float* pf1 = vertices + indices[i + 1] * 3;
                const float* pf2 = vertices + indices[i + 2] * 3;
                ozz::math::Float3 lcl_intersect, lcl_normal;
                if (RayIntersectsTriangle(_ray_origin, _ray_direction,
                    ozz::math::Float3(pf0[0], pf0[1], pf0[2]),
                    ozz::math::Float3(pf1[0], pf1[1], pf1[2]),
                    ozz::math::Float3(pf2[0], pf2[1], pf2[2]),
                    &lcl_intersect, &lcl_normal)) {
                    // Is it closer to start point than the previous intersection.
                    if (!intersected || LengthSqr(lcl_intersect - _ray_origin) <
                        LengthSqr(intersect - _ray_origin)) {
                        intersect = lcl_intersect;
                        normal = lcl_normal;
                    }
                    intersected = true;
                }
            }

            // Copy output
            if (intersected) {
                if (_intersect) {
                    *_intersect = intersect;
                }
                if (_normal) {
                    *_normal = normal;
                }
            }
            return intersected;
        }

        bool RayIntersectsMeshes(const ozz::math::Float3& _ray_origin,
            const ozz::math::Float3& _ray_direction,
            const ozz::span<const ozz::sample::Mesh>& _meshes,
            ozz::math::Float3* _intersect,
            ozz::math::Float3* _normal) {
            bool intersected = false;
            ozz::math::Float3 intersect, normal;
            for (size_t i = 0; i < _meshes.size(); ++i) {
                ozz::math::Float3 lcl_intersect, lcl_normal;
                if (RayIntersectsMesh(_ray_origin, _ray_direction, _meshes[i],
                    &lcl_intersect, &lcl_normal)) {
                    // Is it closer to start point than the previous intersection.
                    if (!intersected || LengthSqr(lcl_intersect - _ray_origin) <
                        LengthSqr(intersect - _ray_origin)) {
                        intersect = lcl_intersect;
                        normal = lcl_normal;
                    }
                    intersected = true;
                }
            }

            // Copy output
            if (intersected) {
                if (_intersect) {
                    *_intersect = intersect;
                }
                if (_normal) {
                    *_normal = normal;
                }
            }
            return intersected;
        }
    }  // namespace sample
}  // namespace ozz
