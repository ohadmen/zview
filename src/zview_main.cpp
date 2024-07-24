#include "src/ui/main_app.h"


static std::vector<std::string> get_args(int argc, char **argv) {
  std::vector<std::string> list;
  for (int i = 1; i != argc; ++i)
    list.push_back(argv[i]);

  return list;
}

int main(int argc, char *argv[]) {
  const auto args = get_args(argc, argv);

  zview::MainApp app;
  app.init();
  app.loadFiles(args);
  app.loop();
  return 0;
}
