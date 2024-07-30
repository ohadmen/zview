#include "src/zview.h"

static std::vector<std::string> get_args(int argc, char **argv) {
  std::vector<std::string> list;
  for (int i = 1; i != argc; ++i) {
    list.push_back(*std::next(argv, i));
  }

  return list;
}

int main(int argc, char *argv[]) {
  const auto files = get_args(argc, argv);

  zview::Zview app;
  if (!app.init()) {
    return 1;
  }
  app.plot(files);
  while (app.loop())
    ;
  return 0;
}
