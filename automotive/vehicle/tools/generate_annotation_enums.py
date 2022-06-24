#!/usr/bin/python

# Copyright (C) 2022 The Android Open Source Project
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
"""A script to generate Java files and CPP header files based on annotations in VehicleProperty.aidl

   Need ANDROID_BUILD_TOP environmental variable to be set. This script will update
   ChangeModeForVehicleProperty.h and AccessForVehicleProperty.h under generated_lib/cpp and
   ChangeModeForVehicleProperty.java and AccessForVehicleProperty.java under generated_lib/java.

   Usage:
   $ python generate_annotation_enums.py
"""
import os
import re
import sys

PROP_AIDL_FILE_PATH = ("hardware/interfaces/automotive/vehicle/aidl/android/hardware/automotive/" +
    "vehicle/VehicleProperty.aidl")
CHANGE_MODE_CPP_FILE_PATH = ("hardware/interfaces/automotive/vehicle/aidl/generated_lib/cpp/" +
    "ChangeModeForVehicleProperty.h")
ACCESS_CPP_FILE_PATH = ("hardware/interfaces/automotive/vehicle/aidl/generated_lib/cpp/" +
    "AccessForVehicleProperty.h")
CHANGE_MODE_JAVA_FILE_PATH = ("hardware/interfaces/automotive/vehicle/aidl/generated_lib/java/" +
    "ChangeModeForVehicleProperty.java")
ACCESS_JAVA_FILE_PATH = ("hardware/interfaces/automotive/vehicle/aidl/generated_lib/java/" +
    "AccessForVehicleProperty.java")

TAB = "    "
RE_ENUM_START = re.compile("\s*enum VehicleProperty \{")
RE_ENUM_END = re.compile("\s*\}\;")
RE_COMMENT_BEGIN = re.compile("\s*\/\*\*")
RE_COMMENT_END = re.compile("\s*\*\/")
RE_CHANGE_MODE = re.compile("\s*\* @change_mode (\S+)\s*")
RE_ACCESS = re.compile("\s*\* @access (\S+)\s*")
RE_VALUE = re.compile("\s*(\w+)\s*=(.*)")

LICENSE = """/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * DO NOT EDIT MANUALLY!!!
 *
 * Generated by tools/generate_annotation_enums.py.
 */

"""

CHANGE_MODE_CPP_HEADER = """#ifndef android_hardware_automotive_vehicle_aidl_generated_lib_ChangeModeForVehicleProperty_H_
#define android_hardware_automotive_vehicle_aidl_generated_lib_ChangeModeForVehicleProperty_H_

#include <aidl/android/hardware/automotive/vehicle/VehicleProperty.h>
#include <aidl/android/hardware/automotive/vehicle/VehiclePropertyChangeMode.h>

#include <unordered_map>

namespace aidl {
namespace android {
namespace hardware {
namespace automotive {
namespace vehicle {

std::unordered_map<VehicleProperty, VehiclePropertyChangeMode> ChangeModeForVehicleProperty = {
"""

CHANGE_MODE_CPP_FOOTER = """
};

}  // namespace vehicle
}  // namespace automotive
}  // namespace hardware
}  // namespace android
}  // aidl

#endif  // android_hardware_automotive_vehicle_aidl_generated_lib_ChangeModeForVehicleProperty_H_
"""

ACCESS_CPP_HEADER = """#ifndef android_hardware_automotive_vehicle_aidl_generated_lib_AccessForVehicleProperty_H_
#define android_hardware_automotive_vehicle_aidl_generated_lib_AccessForVehicleProperty_H_

#include <aidl/android/hardware/automotive/vehicle/VehicleProperty.h>
#include <aidl/android/hardware/automotive/vehicle/VehiclePropertyAccess.h>

#include <unordered_map>

namespace aidl {
namespace android {
namespace hardware {
namespace automotive {
namespace vehicle {

std::unordered_map<VehicleProperty, VehiclePropertyAccess> AccessForVehicleProperty = {
"""

ACCESS_CPP_FOOTER = """
};

}  // namespace vehicle
}  // namespace automotive
}  // namespace hardware
}  // namespace android
}  // aidl

#endif  // android_hardware_automotive_vehicle_aidl_generated_lib_AccessForVehicleProperty_H_
"""

CHANGE_MODE_JAVA_HEADER = """package android.hardware.automotive.vehicle;

import java.util.Map;

public final class ChangeModeForVehicleProperty {

    public static final Map<Integer, Integer> values = Map.ofEntries(
"""

CHANGE_MODE_JAVA_FOOTER = """
    );

}
"""

ACCESS_JAVA_HEADER = """package android.hardware.automotive.vehicle;

import java.util.Map;

public final class AccessForVehicleProperty {

    public static final Map<Integer, Integer> values = Map.ofEntries(
"""

ACCESS_JAVA_FOOTER = """
    );

}
"""


class Converter:

    def __init__(self, name, annotation_re):
        self.name = name
        self.annotation_re = annotation_re

    def convert(self, input, output, header, footer, cpp):
        processing = False
        in_comment = False
        content = LICENSE + header
        annotation = None
        id = 0
        with open(input, 'r') as f:
            for line in f.readlines():
                if RE_ENUM_START.match(line):
                    processing = True
                    annotation = None
                elif RE_ENUM_END.match(line):
                    processing = False
                if not processing:
                    continue
                if RE_COMMENT_BEGIN.match(line):
                    in_comment = True
                if RE_COMMENT_END.match(line):
                    in_comment = False
                if in_comment:
                    match = self.annotation_re.match(line)
                    if match:
                        annotation = match.group(1)
                else:
                    match = RE_VALUE.match(line)
                    if match:
                        prop_name = match.group(1)
                        if prop_name == "INVALID":
                            continue
                        if not annotation:
                            print("No @" + self.name + " annotation for property: " + prop_name)
                            sys.exit(1)
                        if id != 0:
                            content += "\n"
                        if cpp:
                            annotation = annotation.replace(".", "::")
                            content += (TAB + TAB + "{VehicleProperty::" + prop_name + ", " +
                                        annotation + "},")
                        else:
                            content += (TAB + TAB + "Map.entry(VehicleProperty." + prop_name + ", " +
                                        annotation + "),")
                        id += 1

        # Remove the additional "," at the end for the Java file.
        if not cpp:
            content = content[:-1]

        content += footer

        with open(output, 'w') as f:
            f.write(content)


def main():
    android_top = os.environ['ANDROID_BUILD_TOP']
    if not android_top:
        print("ANDROID_BUILD_TOP is not in envorinmental variable, please run source and lunch " +
            "at the android root")

    aidl_file = os.path.join(android_top, PROP_AIDL_FILE_PATH)
    change_mode_cpp_output = os.path.join(android_top, CHANGE_MODE_CPP_FILE_PATH);
    access_cpp_output = os.path.join(android_top, ACCESS_CPP_FILE_PATH);
    change_mode_java_output = os.path.join(android_top, CHANGE_MODE_JAVA_FILE_PATH);
    access_java_output = os.path.join(android_top, ACCESS_JAVA_FILE_PATH);

    c = Converter("change_mode", RE_CHANGE_MODE);
    c.convert(aidl_file, change_mode_cpp_output, CHANGE_MODE_CPP_HEADER, CHANGE_MODE_CPP_FOOTER, True)
    c.convert(aidl_file, change_mode_java_output, CHANGE_MODE_JAVA_HEADER, CHANGE_MODE_JAVA_FOOTER, False)
    c = Converter("access", RE_ACCESS)
    c.convert(aidl_file, access_cpp_output, ACCESS_CPP_HEADER, ACCESS_CPP_FOOTER, True)
    c.convert(aidl_file, access_java_output, ACCESS_JAVA_HEADER, ACCESS_JAVA_FOOTER, False)


if __name__ == "__main__":
    main()