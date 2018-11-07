#
# Copyright (c) 2017 TOYOTA MOTOR CORPORATION
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

project(simple-hs C CXX)

cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME "simple-hs")
set(PROJECT_VERSION "1.0")
set(PROJECT_PRETTY_NAME "SE")
set(PROJECT_DESCRIPTION "Sample application for homescreen and windowmanager")
set(PROJECT_URL "")
set(CMAKE_CXX_FLAGS "-Wall -fpermissive")

#source directory
aux_source_directory(src DIR_SRCS)

#head file path
include_directories(include
$ENV{SDKTARGETSYSROOT}/usr/include/dbus-1.0
$ENV{SDKTARGETSYSROOT}/usr/include/glib-2.0
$ENV{SDKTARGETSYSROOT}/usr/lib/glib-2.0/include
$ENV{SDKTARGETSYSROOT}/usr/lib/dbus-1.0/include/
)

#set extern libraries
SET(LIBRARIES
    libwindowmanager.so
    libm.so
    libjson-c.so
    libhomescreen.so
    librt.so
    libpthread.so
    dbus-1
    dbus-glib-1
)

#add executable file
add_executable(simple-hs ${DIR_SRCS})

#add link library
TARGET_LINK_LIBRARIES(simple-hs ${LIBRARIES})