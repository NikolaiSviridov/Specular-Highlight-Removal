cmake_minimum_required (VERSION 3.8)
PROJECT(HIGHLIGHT)

INCLUDE_DIRECTORIES(
		${HIGHLIGHT_SOURCE_DIR}/src
)

LINK_DIRECTORIES(
		/usr/lib
		/usr/X11R6/lib
)

ADD_EXECUTABLE(
		highlight
		# main code
		src/main.cxx
		src/zHighlightRemoval.cxx
)

TARGET_LINK_LIBRARIES(
		highlight
)
