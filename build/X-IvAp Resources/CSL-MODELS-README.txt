This readme file was written by Denis Maslov (den_rain@steptosky.com) 27.08.2017
last edited 10.09.2017


ABOUT THE CSL MODELS

The csl-model library structure for the x-ivap is based on the xsb files structure which has been originally designed for xsquawkbox.
The original format of the xsb files is still supported by the x-ivap.
Besides this, the x-ivap supports several additional features for the csl-model library.

With xsb files you can setup a kind of mapping between a specific mtl code and a specific 3d object file with specific textures.
Let's say that we have this file structure:
CSL
	B752
		xsb_aircraft.txt
		B752.obj
		XRW.png
		XRW_LIT.png

So, then let's discuss content of the xsb_aircraft.txt file.

Example of minimum content for old object files (version 7 and less) is:
======================================================================================
# package name, you can use any name as package name, but it must be unique within all the csl-packages.
# this name will be expanded by x-ivap up to path to the package folder, so you can use the name as path to the package folder.
EXPORT_NAME __B752

# this line defines an csl-model which will use the B752.obj file as 3d object file.
# without any custom specified textures, the texture specified inside the obj file will be drawn. 
OBJECT __B752:B752.obj
# this line defines mtl code for the csl-model, this sets it to <B752>.
ICAO B752
======================================================================================

Example of maximum content for old object files (version 7 and less) is:
======================================================================================
# package name, you can use any name as package name, but it must be unique within all the csl-packages.
# this name will be expanded by x-ivap up to path to the package folder, so you can use the name as path to the package folder.
EXPORT_NAME __B752

# this line defines an csl-model which will use the B752.obj file as 3d object file. 
OBJECT __B752:B752.obj

# this line defines custom texture for the object file, 
# the object will be drawn with this texture instead of one specified inside the object file.
TEXTURE __B752:XRW.png
# this line defines LIT custom texture
# if there is no this line, the x-ivap will use as lit texture the specified by the line above regular texture with _LIT postfix, 
# or, if there is no any specified custom textures, will use the lit texture specified inside the obj file.
TEXTURE_LIT __B752:XRW.png

# this line specifies the csl-model vertical offset for the clamping feature purposes. 
# read more about this feature below.
VERT_OFFSET 23.6

# these lines can define mtl code for the csl-model.
# you MUST use only one of these lines, not all three or two at the same time.
ICAO B752
AIRLINE B752 XRW
LIVERY B752 XRW WL
======================================================================================

Example of minimum content for modern object files (version 8 or higher) is:
======================================================================================
# package name, you can use any name as package name, but it must be unique within all the csl-packages.
# this name will be expanded by x-ivap up to path to the package folder, so you can use the name as path to the package folder.
EXPORT_NAME __B752

# this line defines an csl-model
OBJ8_AIRCRAFT __B752:B752
# this line defines an object file which will be used as one of 3d object files for this csl-model.
# you can setup several objects for one csl-model, not only one
# SOLID - this defines type of the object as a solid (not transparent) type. 
# Other supported variants of object types: GLASS - a transparent object type, will be drawn with blending.
# YES - this marks the object as an object which has animations, so the sim may not to optimise the object.
# The opposite variant is NO, which allows the sim to optimise the object as an object which has not any animations.
OBJ8 SOLID YES __B752:B752.obj
# this line defines mtl code for the csl-model, it sets it to <B752>.
ICAO B752
======================================================================================

The maximum content example for modern object files (version 8 or higher) is:
======================================================================================
# package name, you can use any name as package name, but it must be unique within all the csl-packages.
# this name will be expanded by x-ivap up to path to the package folder, so you can use the name as path to the package folder.
EXPORT_NAME __B752

# this line defines an csl-model
OBJ8_AIRCRAFT __B752:B752XRW

# these lines define object files which will be used as 3d object files for this csl-model.
# you can setup several objects for one csl-model, not only one
# SOLID - this defines type of the object as a solid (not transparent) type. 
# Other supported variants of object types: GLASS - a transparent object type.
# YES - this marks the object as an object which has animations, so the sim may not to optimise the object.
# The opposite variant is NO, which allows the sim to optimise the object as an object without animations.
OBJ8 SOLID YES __B752:B752_gear.obj
OBJ8 GLASS NO __B752:B752_glass.obj
# also, you can specify custom textures for the csl-model, only regular one for example.
# the texture specified in this line with added _LIT postfix will be used as lit texture in this case.
OBJ8 SOLID NO __B752:B752_fuse.obj XRW.png
# but you can specify lit texture manually with this variant.
OBJ8 SOLID YES __B752:B752_wings.obj XRW.png XRW_LIT.png

# this line specifies the csl-model vertical offset for the clamping feature purposes. 
# read more about this feature below.
VERT_OFFSET -17.5

# these lines can define mtl code for the csl-model.
# you MUST use only one of these lines, not all three or two at the same time.
ICAO B752
AIRLINE B752 XRW
LIVERY B752 XRW WL
======================================================================================


THE CLAMPING FEATURE
It's designed to put csl-model onto the ground accuratelly, regardless to the actual airport elevation of a multiplayer pilot.
It probes local terrain to find exactly local elevation under a multiplayer plane, and uses it to put the plane onto this elevation additionally using the csl-model vertical offset.
The csl-model vertical offset is a distance between the origin point of the object and the lowest point of the object (usually the bottom of gears) along a vertical axis (the y axis in the sim) in meters.
The csl-model vertical offset MUST BE setup correctly otherwise the feature CANNOT GARANTY that the multiplayer planes will be put onto the ground accuratelly.
You can setup the offset via three ways which is explained below in decreasing order of priority:
1. User (local) offsets.
	A user can setup its local offsets via a specific x-ivap command or via userVertOffsets.txt file.
	The format of a line is:
	<Mtl code>, <offset in meters>
	B752XRWWL, -13.5
2. Xsb offsets, which can be specified in the xsb files by all csl-model developers with the command VERT_OFFSET.
	You can see examples of this command above.
3. If no one of above ways is used, then x-ivap will try to calculate the offset automatically from the object files which are used for the csl-model.
	WARNING: If the object has translate and/or rotate animations, the calculation result MAY BE WRONG.
	It can be wrong because the alghorytm of the calculation is very simple, and cannot parse any rotation/translation animations, any parenting/translations and etc.
	So, the main requirement to make calculation result correct is exporting objects where coordinates of the lowest point are relative to the object origin point.
	Or you can use one of the above ways.