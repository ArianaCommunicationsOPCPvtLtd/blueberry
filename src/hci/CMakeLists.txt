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
## Target Device:  Raspberry Pi
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

# add the executable
include_directories(include)
file(GLOB all_SRCS
	"${PROJECT_SOURCE_DIR}/src/hci/*.cpp"
	)
add_library(hci OBJECT ${all_SRCS})
