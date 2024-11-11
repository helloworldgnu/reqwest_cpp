#include "main_window.hpp"
#include <QVBoxLayout>
#include <exception>
#include <iostream>
#include <ostream>
#include <vector>

#include "base/net/http/client.h"
#include "base/net/http/client_builder.h"
#include "base/net/http/header_map.h"
#include "base/net/http/request_builder.h"
#include "base/net/http/response.h"

base::net::http::Client::uptr clinet = nullptr;

void MainWindow::test_full()
{
    auto headerMap = base::net::http::HeaderMap::Build();
    headerMap->insert("default", "value");
    if (!clinet)
    {
        auto client_builder = base::net::http::ClientBuilder::Build();
        if (client_builder)
        {
            clinet = client_builder->user_agent("Rust/1.0.0")
                         ->default_headers(std::move(headerMap))
                         //                     ->default_headers({{"de", "he"}})
                         ->redirect(10)
                         //->proxy(ffi::proxy::http("http://192.168.1.37:8888"))
                         ->timeout(nullptr)
                         ->build();
        }
    }

    auto request_builder = clinet->get("http://192.168.1.29:8023/c9/xx");
    if (request_builder)
    {
        base::net::http::Response::uptr resp =
            //                                          ->basic_auth("admin", "password")
            request_builder
                ->header("Test1", "abv")
                //                                          ->header("Test2", "abv")
                //                                          ->query({{"3", "4"}, {"5", "6"}})
                ->body("123456")
                //                             ->json({{"name","markrenChina"}})
                //                             ->json("{\"test\":123}")
                //->file_body("rest_client.log")
                ->timeout(1000)
                ->send();

        auto headermap2 = resp->headers();
        std::cout << headermap2->get("content-type") << std::endl;
        std::cout << headermap2->keys() << std::endl;
        std::cout << headermap2->values() << std::endl;
    }
}

void MainWindow::test_download()
{
    //  auto *kind = new uint32_t(0);
    //  auto *value = new int32_t(0);
    //  try {
    //    if (!clinet) {
    //      clinet = ffi::ClientBuilder::New()
    //                   ->user_agent("Rust/1.0.0")
    //                   ->build();
    //    }
    //
    //    ffi::Response *resp =
    //        clinet
    //            ->get("https://github.com/Eugeny/tabby/releases/download/v1.0.210/"
    //                  "tabby-1.0.210-macos-arm64.dmg")
    //            ->send(kind, value);
    //
    //    static constexpr int buf_size = 4 * 1024;
    //    static int times{};
    //    auto buf = new uint8_t[buf_size];
    //    int read_bytes = 0;
    //    do {
    //      memset(buf, 0, buf_size);
    //
    //      read_bytes = resp->read(buf, buf_size, kind);
    //      if (read_bytes > 0) {
    //        times++;
    //        std::cout << "reading " << read_bytes << " times " << times
    //                  << std::endl;
    //      }
    //    } while (read_bytes > 0);
    //
    //  } catch (const ffi::WrapperException &e) {
    //    std::cout << e.what() << std::endl;
    //    std::cout << "kind value:" << *kind << std::endl;
    //  }
}

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    auto button = new QPushButton("test");
    auto button1 = new QPushButton("test download");
    auto layout = new QVBoxLayout();
    layout->addWidget(button);
    layout->addWidget(button1);
    setLayout(layout);

    connect(button, SIGNAL(clicked()), this, SLOT(test_full()));
    connect(button1, SIGNAL(clicked()), this, SLOT(test_download()));
}