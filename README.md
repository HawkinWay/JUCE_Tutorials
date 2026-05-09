# How to use

## 1. clone the repository

```bash
git clone https://github.com/HawkinWay/JUCE_Tutorials.git
```

## 2. add JUCE framework

```bash
# locate the directory you cloned, and then navigate that direcory in your terminal
cd JUCE_Tutorials/

# use '--depth 1' to shallow cloning
git clone --depth 1 https://github.com/juce-framework/JUCE.git
```

## 3. add your own project

```bash
mkdir projectFileName

touch CMakeLists.txt
```

## 4. create include files and source files

```bash
mkdir inclue source

cd include
touch projectName.h

cd ../source
touch main.cpp
```

## 5. edit CMakeLists.txt in projectFileName

```CMakeLists.txt
juce_add_gui_app(projectName
		PRODUCT_NAME "projectName"
)

target_sources(projectName
		PRIVATE
		include/projectName.h
		source/main.cpp
)

target_link_libraries(projectName
		PRIVATE
		# add the corresponding juce module
    # such as juce::juce_audio_utils
)

juce_generate_juce_header(projectName)
```

## 6. configure CMake

using your IDE to configure CMake  
CLion is recommended
