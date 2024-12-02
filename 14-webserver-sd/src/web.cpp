#include "web.h"

#define WEB_INDEX "index.html"
#define WEB_404 "/404.html"
#define WEB_FAVICON "/favicon.ico"
#define WEB_MIME_PLAIN "text/plain"

bool getMime(String path, char out[])
{
    String sout = "";
    if      (path.endsWith(".html")) sout = "text/html";
    else if (path.endsWith(".htm"))  sout = "text/html";
    else if (path.endsWith(".css"))  sout = "text/css";
    else if (path.endsWith(".xml"))  sout = "text/xml";
    else if (path.endsWith(".txt"))  sout = WEB_MIME_PLAIN;
    else if (path.endsWith(".js"))   sout = "application/javascript";
    else if (path.endsWith(".png"))  sout = "image/png";
    else if (path.endsWith(".jpg"))  sout = "image/jpeg";
    else if (path.endsWith(".jpeg")) sout = "image/jpeg";
    else if (path.endsWith(".gif"))  sout = "image/gif";
    else if (path.endsWith(".svg"))  sout = "image/svg+xml";
    else if (path.endsWith(".ico"))  sout = "image/x-icon";
    else if (path.endsWith(".zip"))  sout = "application/zip";
    else if (path.endsWith(".gz"))   sout = "application/gzip";
    else if (path.endsWith(".7z"))   sout = "application/x-7z-compressed";
    else if (path.endsWith(".pdf"))  sout = "application/pdf";
    else if (path.endsWith(".json")) sout = "application/json";
    else if (path.endsWith(".bin"))  sout = "application/octet-stream";
    else if (path.endsWith(".mid"))  sout = "audio/midi";
    else if (path.endsWith(".midi")) sout = "audio/midi";
    else if (path.endsWith(".mp3"))  sout = "audio/mpeg";
    else if (path.endsWith(".mp4"))  sout = "video/mp4";
    else if (path.endsWith(".mpeg")) sout = "video/mpeg";
    else if (path.endsWith(".ttf"))  sout = "font/ttf";

    // valid mime?
    if (sout != "")
    {
        strcpy(out, sout.c_str()); // copy to out
        return true;
    }
    return false;
}

void tWeb::sendError(AsyncWebServerRequest* req, String msg)
{
    req->send(500, WEB_MIME_PLAIN, msg);
}

bool tWeb::handleFile(AsyncWebServerRequest* req, String path, int code)
{
    // can't begin sd card?
    if (!m_sd->begin())
        return false;

    // open file
    File f = m_sd->openRead(path);
    if (!f) // can't open file?
    {
        m_sd->end(); // end sd access
        return false;
    }
    // is f a directory ?
    if (f.isDirectory())
    {
        f.close(); // close directory
        path = path + ("/" WEB_INDEX); // update path
        f = m_sd->openRead(path); // open index.html inside folder
        if (!f) // file does not exist?
        {
            m_sd->end(); // end sd access
            return false;
        }
    }
    // buffer for mime type string
    char mime[32];
    if (!getMime(path, mime)) // invalid mime type?
    {
        //Serial.println("handleFile: invalid mime: " + String(mime));
        f.close(); // close file
        m_sd->end(); // end sd access
        return false;
    }
    // end card usage
    m_sd->end();

    // Create response (the function inside will run once per chunk until the whole file is sent)
    AsyncWebServerResponse* resp = req->beginChunkedResponse(mime, [&f, this, path](uint8_t* buf, size_t maxlen, size_t index) -> size_t
    {
        if (!m_sd->begin()) // Can't start card?
        {
            Serial.println("beginChunkedResponse: No SD");
            return 0;
        }
        File f = m_sd->openRead(path); // open file
        if (!f) // cant open file?
        {
            Serial.println("beginChunkedResponse: Failed to open file");
            m_sd->end(); // end sd card
            return 0;
        }
        if (!f.available()) // file unavailable?
        {
            Serial.println("beginChunkedResponse: File not available?");
            f.close(); // close file
            m_sd->end(); // end sd card
            return 0;
        }
        f.seek(index); // set read position to index
        size_t len = f.read(buf, maxlen); // read chunk
        f.close(); // close file
        m_sd->end(); // end sd access
        return len;
    });
    resp->setCode(code); // set response code

    req->send(resp); // send response
    return true;
}

void tWeb::handleReq(AsyncWebServerRequest* req)
{
    // not GET?
    if (!String("GET").equals(req->methodToString()))
    {
        // send error
        sendError(req, "Invalid method, only accepts GET.");
        return;
    }
    // get url
    String path = req->url();
    Serial.println("GET: " + path);
    // fix path
    if (path.endsWith("/")) 
        path = (WEB_ROOT + path + WEB_INDEX);
    else path = (WEB_ROOT + path);
    // could not handle file?
    if (!handleFile(req, path))
        handle404(req); // handle 404
}

void tWeb::handle404(AsyncWebServerRequest* req)
{
    // can't find file 404.html?
    if (!handleFile(req, (WEB_ROOT WEB_404), 404))
        req->send(404, WEB_MIME_PLAIN, "Page not found"); // send in text
}

void tWeb::handleFavicon(AsyncWebServerRequest* req)
{
    char favicon[128];

    if (m_sd->readAllText(favicon, 128, WEB_ROOT "/favicon.txt"))
    {
        String sfavicon(favicon); // Create ArduinoLib String from buffer
        sfavicon = WEB_ROOT + sfavicon;
        sfavicon.trim(); // Remove white spaces
        //Serial.println("handleFavicon: " + sfavicon);
        if (!handleFile(req, sfavicon)) // can't find path inside favicon.txt?
            handle404(req); // send not found
        return;
    }
    //Serial.println("handleFavicon: " WEB_FAVICON);
    if (!handleFile(req, (WEB_ROOT WEB_FAVICON))) // can't find favicon.ico?
        handle404(req); // send not found
}

void tWeb::init()
{
    // When not registered, try to find webpages on sd card
    m_server.onNotFound([this](AsyncWebServerRequest* r){this->handleReq(r);});
    // Register getter for favicon.ico
    m_server.on(WEB_FAVICON, [this](AsyncWebServerRequest* r){this->handleFavicon(r);});
    // Begin server
    m_server.begin();
}

