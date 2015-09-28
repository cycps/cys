#!/usr/bin/env bash

SHELL=/bin/bash

BLUE=\e[34m
GREEN=\e[32m
DEFAULT=\e[39m

.PHONY: all
all: sundials boost yaml 

#yaml --------------------------------------------------------------------------
.PHONY: yaml
yaml: boost yaml-cpp-release-0.5.2
	@printf "${BLUE}Installing libyaml${DEFAULT}\n"
	@cd yaml-cpp-release-0.5.2; \
		cmake \
			-DCMAKE_CXX_COMPILER=clang++ \
			-DCMAKE_C_COMPILER=clang \
			-DCMAKE_CXX_FLAGS="-stdlib=libc++" \
			. > /dev/null && \
		make > /dev/null && \
		make install > /dev/null
	
	@printf "${GREEN}Finished installing libyaml${DEFAULT}\n"

yaml-cpp-release-0.5.2: yaml-cpp-release-0.5.2.tar.gz
	@printf "${BLUE}Unpacking libyaml${DEFAULT}\n"
	@tar xzf yaml-cpp-release-0.5.2.tar.gz

yaml-cpp-release-0.5.2.tar.gz:
	@printf "${BLUE}Getting libyaml-0.5.2 from github${DEFAULT}\n"
	@curl -Ls \
		-o yaml-cpp-release-0.5.2.tar.gz \
		https://github.com/jbeder/yaml-cpp/archive/release-0.5.2.tar.gz > /dev/null

#boost -------------------------------------------------------------------------
.PHONY: boost
boost: boost_1_58_0
	@printf "${BLUE}Installing required boost libraries${DEFAULT}\n"
	
	@cd boost_1_58_0; \
		./bootstrap.sh \
			--with-toolset=clang --with-libraries=program_options,filesystem,system \
			mcxxflags="-std=c++1y -stdlib=libc++" \
			linkflags="-stdlib=libc++" > /dev/null && \
		./b2 \
			toolset=clang \
			cxxflags="-std=c++1y -stdlib=libc++" \
			linkflags="-stdlib=libc++" \
			-j 4 stage release  > /dev/null && \
		./b2 install \
			toolset=clang \
			cxxflags="-std=c++1y -stdlib=libc++" \
			linkflags="-stdlib=libc++" > /dev/null
	
	@printf "${GREEN}Finished installing boost libraries${DEFAULT}\n"
	
boost_1_58_0: boost_1_58_0.tar.gz
	@printf "${BLUE}Unpacking boost${DEFAULT}\n"
	@tar xzf boost_1_58_0.tar.gz

boost_1_58_0.tar.gz:
	@printf "${BLUE}Getting boost-1.58 from bintray${DEFAULT}\n"
	@curl -Ls \
		-o boost_1_58_0.tar.gz \
		https://bintray.com/artifact/download/cycps/core-dependencies/boost_1_58_0.tar.gz

#libxml ------------------------------------------------------------------------
.PHONY: libxml
libxml: libxml2-2.7.2
	@printf '$(BLUE)%s$(DEFAULT)\n' "Installing libxml" 
	
	@cd libxml2-2.7.2; \
		./configure > /dev/null && \
		make > /dev/null && \
		make install > /dev/null
	
	@printf "${GREEN}Finished installing libxml${DEFAULT}\n"

libxml2-2.7.2: libxml2-2.7.2.tar.gz
	@printf "${BLUE}Unpacking libxml${DEFAULT}\n"
	@tar xzf libxml2-2.7.2.tar.gz 

libxml2-2.7.2.tar.gz:
	@printf "${BLUE}Getting libxml2-2.7.2 from bintray${DEFAULT}\n"
	@curl -Ls \
		-o libxml2-2.7.2.tar.gz \
		https://bintray.com/artifact/download/cycps/core-dependencies/libxml2-2.7.2.tar.gz

#mpich -------------------------------------------------------------------------
.PHONY: mpich
mpich: libxml mpich-3.1.4
	@printf "${BLUE}Installing libmpich (this one takes a while :/)${DEFAULT}\n"
	
	@cd mpich-3.1.4; \
		./configure --disable-fortran > /dev/null && \
		make -j4 > /dev/null && \
		make install > /dev/null
	
	@printf "${GREEN}Finished installing libmpich${DEFAULT}\n"

mpich-3.1.4: mpich-3.1.4.tar.gz
	@printf "${BLUE}Unpacking mpich${DEFAULT}\n"
	@tar xzf mpich-3.1.4.tar.gz

mpich-3.1.4.tar.gz:
	@printf "${BLUE}Getting mpich-3.1.4 from bintray${DEFAULT}\n"
	@curl -Ls \
		-o mpich-3.1.4.tar.gz \
		https://bintray.com/artifact/download/cycps/core-dependencies/mpich-3.1.4.tar.gz

#sundials ----------------------------------------------------------------------
.PHONY: sundials
sundials: mpich sundials-2.6.2
	@printf "${BLUE}Installing sundials${DEFAULT}\n"
	
	@cd sundials-2.6.2; \
    mkdir build && cd build \
		cmake .. -G Ninja > /dev/null && \
		ninja > /dev/null && \
		ninja install > /dev/null
	
	#@bash apply_patches.sh > /dev/null \
	
	@printf "${GREEN}Finished installing sundials${DEFAULT}\n"

sundials-2.6.2: sundials-2.6.2.tar.gz
	@printf "${BLUE}Unpacking sundials${DEFAULT}\n"
	@tar xzf sundials-2.6.2.tar.gz

sundials-2.6.2.tar.gz:
	@printf "${BLUE}Getting sundials-2.6.2 from bintray${DEFAULT}\n"
	@curl -Ls \
		-o sundials-2.6.2.tar.gz \
		https://bintray.com/artifact/download/cycps/core-dependencies/sundials-2.6.2.tar.gz


.PHONY: clean
clean:
	rm -rf sundials*
	rm -rf libxml*
	rm -rf boost*
	rm -rf mpich*
	rm -rf yaml*

