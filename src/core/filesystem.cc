// Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "src/core/filesystem.h"

#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/platform/env.h"

namespace nvidia { namespace inferenceserver {

Status
GetSubdirs(const std::string& path, std::set<std::string>* subdirs)
{
  std::vector<std::string> childs;
  RETURN_IF_TF_ERROR(tensorflow::Env::Default()->GetChildren(path, &childs));

  // GetChildren() returns all descendants instead for cloud storage
  // like GCS. In such case we should filter out all non-direct
  // descendants.
  std::set<std::string> real_childs;
  for (const std::string& child : childs) {
    real_childs.insert(child.substr(0, child.find_first_of('/')));
  }

  for (const auto& child : real_childs) {
    const auto vp = tensorflow::io::JoinPath(path, child);
    if (tensorflow::Env::Default()->IsDirectory(vp).ok()) {
      subdirs->insert(child);
    }
  }

  return Status::Success;
}

Status
GetFiles(const std::string& path, std::set<std::string>* files)
{
  std::vector<std::string> childs;
  RETURN_IF_TF_ERROR(tensorflow::Env::Default()->GetChildren(path, &childs));

  // GetChildren() returns all descendants instead for cloud storage
  // like GCS. In such case we should filter out all non-direct
  // descendants.
  std::set<std::string> real_childs;
  for (const std::string& child : childs) {
    real_childs.insert(child.substr(0, child.find_first_of('/')));
  }

  for (const auto& child : real_childs) {
    const auto vp = tensorflow::io::JoinPath(path, child);
    if (!tensorflow::Env::Default()->IsDirectory(vp).ok()) {
      files->insert(child);
    }
  }

  return Status::Success;
}

}}  // namespace nvidia::inferenceserver
