cmake_minimum_required(VERSION 2.6)

project(Demo)

set(OGRE_SYSTEMS "../src")

# Заголовочные файлы систем Ogre
include_directories(${OGRE_SYSTEMS})

# Ядро системы - логи, исключения, управление памятью, 
# динамически загружаемыми библиотеками и плагинами
add_subdirectory(Core)

# Плагины
add_subdirectory(Plugins)

# Консольный загрузчик плагинов
add_subdirectory(Loader)

# Графический менеджер плагинов
# add_subdirectory(QtGuiLoader)

# Скриптовая подсистема
# add_subdirectory(ScriptEngine)