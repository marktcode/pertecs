#pertecs

PERTECS (Programmable Environment for Real-Time emulation of Continuous Systems) is a suite of tools. 
At the core of this suite is an executable UNIX tool, configured using the language of analogue computer, 
to give flexible real-time data processing capabilitie.s
 
 This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

To compile for Mac
#-mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.5.sdk
cc -mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.5.sdk -lexpat -lm -framework vecLib -framework OpenGL -framework GLUT pertecs_main.c pertecs_trace.c spod_surface.c spod_display.c pertecs_init.c pertecs_modules.c pertecs_trees.c pertecs_entropy.c pertecs_xml.c  pertecs_connections_xml.c mkfilter.c complex.c pertecs_OSC.c htmsocket.c OSC_client.c OSC_timeTag.c -O3 -o pertecs

#for the pi
cc -lexpat -lm -lpthread -lglut -lGLU -lGL -lm -ffast-math -lX11 -lXext -lXt -lSM -lICE pertecs_main.c pertecs_init.c pertecs_modules.c pertecs_trees.c pertecs_entropy.c pertecs_xml.c  pertecs_connections_xml.c spod_display.c spod_surface.c pertecs_trace.c mkfilter.c complex.c pertecs_OSC.c htmsocket.c OSC_client.c OSC_timetag.c -O3 -o pertecs -lbcm2835

