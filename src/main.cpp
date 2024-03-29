#include <gempyre.h>
#include "treeview_resource.h"
#include <gempyre_utils.h>
#include <vector>
#include <tuple>
#include <set>
#include <algorithm>

using StateValue = std::tuple<std::string, std::string>;
struct idComp {
    bool operator ()(const StateValue& a, const StateValue& b) const {
        return std::get<0>(a) < std::get<0>(b);
    }
};
using Entries = std::vector<std::tuple<std::string, bool>>;
using State = std::set<StateValue, idComp>;

const std::string openClass {"open tree"};
const std::string closeClass {"closed tree"};
const std::string fileClass {"file tree"};

Entries readdir(const std::string& dir, bool showHidden) {
    Entries entries;
    for(const auto& path: GempyreUtils::directory(dir)) {
        const auto fullname = dir + "/" + path;
        if(path != "." && path != ".." && (showHidden || !GempyreUtils::is_hidden_entry(fullname)))
            entries.emplace_back(path, GempyreUtils::is_dir(fullname));
    }
    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b){return std::get<0>(a) < std::get<0>(b);});
    return entries;
}

std::string decoderate(const std::string& str, const std::string dec) {
    return "<" + dec + ">" + str + "</" + dec + ">";
}

Gempyre::Element addDir(Gempyre::Ui& ui, Gempyre::Element& root, const std::string& rootnamec, State& openDirs, bool showHidden) {
    const auto rootname = rootnamec.back() != '/' ? rootnamec + '/' : rootnamec;
    const auto childs = readdir(rootname, showHidden);
    auto parent = Gempyre::Element(ui, rootname + "_ul", "UL", root);
    parent.set_attribute("class", "tree");
    for(const auto& c : childs) {
        const auto isDir = std::get<1>(c);
        const auto basename = std::get<0>(c);
        const auto fullname = rootname + basename + (isDir ? "/" : "");
        const auto id = GempyreUtils::substitute(fullname, " ", "_");
        auto current = Gempyre::Element(ui, id, "LI", parent)
            .subscribe("click", [&ui, root, &openDirs, showHidden, fullname, basename] (const Gempyre::Event& ev) {
                Gempyre::Element el(ev.element);
                const auto attVal = el.attributes();
                if(attVal.has_value()) {
                    const auto att = attVal.value();
                    if(att.find("class")->second == closeClass) {

                        const auto path = att.find("name")->second;
                        addDir(ui, el, path, openDirs, showHidden);
                        el.set_attribute("class", openClass);
                        openDirs.emplace(el.id(), path);
                    } else if(att.find("class")->second == openClass) {
                        const auto childrenVal = el.children();
                        if(childrenVal.has_value()) {
                            auto children = childrenVal.value();
                            for(auto& e : children) {
                                const auto a = e.attributes();
                                e.remove();
                            }
                            el.set_attribute("class", closeClass);
                            auto it = openDirs.find(std::make_tuple(el.id(), ""));
                            if(it != openDirs.end())
                                openDirs.erase(it);
                        }
                    } else { //file
                            ui.open(ui.address_of(fullname), basename);
                    }
                }
            })
            .set_attribute("class", isDir ? closeClass : fileClass)
            .set_attribute("name", fullname)
            .set_html(isDir ? basename : basename);
             auto it = openDirs.find(std::make_tuple(id, ""));
             if(it != openDirs.end()) {
                 current.set_attribute("class", openClass);
                 addDir(ui, current, std::get<1>(*it), openDirs, showHidden);
             }
    }
    return parent;
}

int main(int argc, char** argv) {
    Gempyre::set_debug();
    Gempyre::Ui ui({{"/treeview.html", Treeviewhtml}, {"/tree.css", Treecss}, {"/favicon.ico", Faviconico}}, "treeview.html");
    const std::string root = argc > 1 ? argv[1]  :
#ifdef WINDOWS_OS
	"C:/";
#else
	"/";
#endif
    Gempyre::Element holdingElement(ui, "treeview");
    State openPages; //here we store the state or UI, it keeps track on opened folders and restored as needed
    auto showHidden = false;

    std::function<void()> reload;

    reload = [&ui, &reload, &openPages, root, holdingElement, &showHidden]() mutable { //this function constructs the ui
         auto rootElement = addDir(ui, holdingElement, root, openPages, showHidden);
         Gempyre::Element(ui, "name").set_html(GempyreUtils::host_name());
         Gempyre::Element(ui, "hiddenbox").subscribe("checked", [&reload, &showHidden, rootElement](const Gempyre::Event& el) mutable {
             const auto values = el.element.values();
             if(values.has_value()) {
                 const auto bstr = values.value().at("checked");
                 showHidden = bstr == "true";
                 rootElement.remove();
                 reload();
             }
         });
        };

    reload(); //intial load

    ui.on_reload(reload);
   // ui.onUiExit(nullptr);
    ui.run();
}


