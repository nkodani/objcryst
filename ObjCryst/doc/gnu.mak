include ../rules.mak
DIR_CRYST := ..

#target to make documentation (requires doxygen)
#also makes tags file, although it is not related to doxygen
doc:
	doxygen doxygen.config
	cp ${DIR_CRYST}/license.txt html/ObjCryst-license.txt
	cp ${DIR_CRYST}/../newmat/nm10.htm html/newmat.htm
	cp ${DIR_CRYST}/../atominfo/readme.html html/atominfo.html
	cp ${DIR_CRYST}/../sglite/LICENSE html/sglite-license.txt
   
ctags:
	ctags ${DIR_LIBCRYST}/*.cpp ${DIR_VFNQUIRKS}/*.cpp ${DIR_SGLITE}/*.c  ${DIR_ATOMINFO}/*.c ${DIR_REFOBJ}/*.cpp ${DIR_CRYSTVECTOR}/*.cpp ${DIR_WXWCRYST}/*.cpp ${DIR_LIBCRYST}/*.h  ${DIR_VFNQUIRKS}/*.h ${DIR_SGLITE}/*.h  ${DIR_ATOMINFO}/*.h  ${DIR_CRYSTVECTOR}/*.h ${DIR_REFOBJ}/*.h ${DIR_CRYSTVECTOR}/*.h ${DIR_WXWCRYST}/*.h

clean:
	@${RM} html/* latex/*
