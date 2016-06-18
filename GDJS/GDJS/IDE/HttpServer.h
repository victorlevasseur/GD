/*
 * GDevelop JS Platform
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the MIT License.
 */
#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <string>
#include "GDCore/String.h"
struct mg_context;

namespace gdjs
{

/**
 * \brief A very simple web server.
 *
 * Basically a wrapper around mongoose ( https://github.com/valenok/mongoose ).
 */
class HttpServer
{
public:
    /**
     * \brief Default constructor doing nothing.
     *
     * Run the webserver using HttpServer::Run.
     */
    HttpServer() : ctx(NULL) {};

    /**
     * \brief Destroy the web server, stopping it if it was running.
     */
    virtual ~HttpServer();

    /**
     * \brief Run a web server, on port 2828, which index is located at \a indexDirectory.
     * \param indexDirectory The root of the webserver.
     */
    void Run(gd::String indexDirectory);

    /**
     * \brief Stop the webserver if it was running
     */
    void Stop();

private:
    struct mg_context * ctx;
};

}
#endif // HTTPSERVER_H
#endif
