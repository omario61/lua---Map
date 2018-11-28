package = "map"
version = "1.1-0"

source = {
   url = "git://github.com/clementfarabet/lua---camera",
}

description = {
   summary = "A simple wrapper package to give torch access to a c++ map",
   detailed = [[
   ]],
   homepage = "https://github.com/clementfarabet/lua---camera",
   license = "BSD"
}

dependencies = {
   "torch >= 7.0",
   "xlua >= 1.0",
   "sys >= 1.0",
   "image >= 1.0.1"
}

build = {
   type = "command",
   build_command = [[
cmake -E make_directory build;
cd build;
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$(LUA_BINDIR)/.." -DCMAKE_INSTALL_PREFIX="$(PREFIX)";
$(MAKE)
   ]],
   install_command = "cd build && $(MAKE) install"
}
