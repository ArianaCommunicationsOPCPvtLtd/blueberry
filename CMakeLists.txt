################################################################################
##             __                                            __
##            /  \       ___    _       _      ___          /  \
##           /    \     |   |  | |     / \    |   \        /    \
##          /  /\  \    |  /   | |    /   \   |    \      /  /\  \
##         /  /  \  \   |  \   | |   /  _  \  | |\  \    /  /  \  \
##        /  /    \  \__|   \__| |__/  / \  \_| | \  \__/  /    \  \
##       /  /      \______/\__________/   \_____|  \______/      \  \
##      /  /  _  _                        _     ___    _        _ \  \
##  ___/  /  |  | | |\/| |\/| | | |\ | | |   /\  |  | | | |\ | |_  \  \___
##  \____/   |_ |_| |  | |  | |_| | \| | |_ /--\ |  | |_| | \|  _|  \____/
##  
## Company:        Ariana Communications OPC Private Limited
## Copyright (C) 2020 Ariana Communications - www.ariana-communications.com
##
## Module Name:    CMakeLists.txt
## Project Name:   Blueberry
## Tool versions:  GNU CMake
## Description:    The cmake config for Blueberry
##
## Dependencies:
## 
## Revision:
## Revision 0.1 - File Created
## Additional Comments:
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
## A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
## OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
## LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
## PLEASE REFER TO THE LICENSE INCLUDED IN THIS DISTRIBUTION.
## 
################################################################################
cmake_minimum_required(VERSION 3.9)

# set the project name
project(blueberry VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(blueberry_config.h.in ${PROJECT_SOURCE_DIR}/include/blueberry_config.h)
set(BT_HAVE_UART "ON")
set(BT_HAVE_USB "OFF")
set(BT_HAVE_HIDP "ON")
set(BT_HAVE_SDP "OFF")
set(BT_HAVE_GATT "OFF")
set(BT_HAVE_ATT "OFF")
set(BT_HAVE_SMP "OFF")
set(BT_HAVE_RFCOMM "OFF")

# specify compiler specifications
set(CMAKE_SYSTEM_NAME none)
set(CMAKE_SYSTEM_PROCESSOR cortex-a53)
set(arch armv8-a)
set(fpu neon-fp-armv8)
set(float-abi softfp)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_C_FLAGS "-march=${arch} -mcpu=${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_CXX_FLAGS "-march=${arch} -mcpu=${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_RANLIB arm-none-eabi-ranlib)

# add the executable
include_directories(include)
add_subdirectory(src)

set(all_LIBS $<TARGET_OBJECTS:hci>)
list(APPEND all_LIBS $<TARGET_OBJECTS:common>)
list(APPEND all_LIBS $<TARGET_OBJECTS:l2cap>)
if(BT_HAVE_UART STREQUAL "ON")
	list(APPEND all_LIBS $<TARGET_OBJECTS:uart>)
endif(BT_HAVE_UART STREQUAL "ON")
if(BT_HAVE_USB STREQUAL "ON")
	list(APPEND all_LIBS $<TARGET_OBJECTS:usb>)
endif(BT_HAVE_USB STREQUAL "ON")
if(BT_HAVE_HIDP STREQUAL "ON")
	list(APPEND all_LIBS $<TARGET_OBJECTS:hidp>)
endif(BT_HAVE_HIDP STREQUAL "ON")
if(BT_HAVE_SDP STREQUAL "ON")
	list(APPEND all_LIBS $<TARGET_OBJECTS:sdp>)
endif(BT_HAVE_SDP STREQUAL "ON")
if(BT_HAVE_GATT STREQUAL "ON")
	list(APPEND all_LIBS $<TARGET_OBJECTS:gatt>)
endif(BT_HAVE_GATT STREQUAL "ON")
if(BT_HAVE_ATT STREQUAL "ON")
	list(APPEND all_LIBS $<TARGET_OBJECTS:att>)
endif(BT_HAVE_ATT STREQUAL "ON")
if(BT_HAVE_SMP STREQUAL "ON")
	list(APPEND all_LIBS $<TARGET_OBJECTS:smp>)
endif(BT_HAVE_SMP STREQUAL "ON")
if(BT_HAVE_RFCOMM STREQUAL "ON")
	list(APPEND all_LIBS $<TARGET_OBJECTS:rfcomm>)
endif(BT_HAVE_RFCOMM STREQUAL "ON")
add_library(blueberry ${all_LIBS})

# install the library
install(TARGETS blueberry ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include/blueberry
	FILES_MATCHING PATTERN "*.h")
