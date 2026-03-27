#include "WiFi_Downloader.h"
#include <SD.h>
#include <stdint.h>
#include <WiFi.h>
#include <WebServer.h>

static WebServer server(80);
static uint8_t sdUploadBuf[16384];


static String getWebpage() {
  return R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        :root {
            --body-bg: radial-gradient(circle, #db143f 0%, #941d58 100%);
            --body-color: #831843;
            --card-bg: rgba(255, 250, 245, 0.95);
            --card-bx-shadow: 0 25px 50px rgba(70, 3, 30, 0.4);
            --subtitle-color: #b54a7a;
            --drop-zone-border: 4px dashed #ff2e97;
            --drop-zone-bg: #ffe4f1;
            --drop-zone-dragover-bg: #ffd1e8;
            --drop-zone-i-color: #ff2e97;
            --progress-bg: #c9c2c2;
            --bar-bg: linear-gradient(90deg, #ff2e97, #ff85c1);
        }

        :root:has(#theme-toggle:checked) {
            --body-bg: #0f172a;
            --body-color: rgb(200, 210, 253);
            --card-bg: #1e293b;
            --card-bx-shadow: 0 25px 50px rgba(0, 0, 0, .4);
            --subtitle-color: #4a78b5;
            --drop-zone-border: 4px dashed #294f81;
            --drop-zone-bg: #121e3a;
            --drop-zone-dragover-bg: #142242;
            --drop-zone-i-color: rgb(81, 193, 201);
            --progress-bg: #334155;
            --bar-bg: #22c55e;
        }

        body {
            background: var(--body-bg);
            color: var(--body-color);
            font-family: Arial, sans-serif, Helvetica;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            margin: 0;
            flex-direction: column;
            transition: background 0.3s ease;
            /* Required for the splash text */
        }

        .card {
            position: relative;
            /* background: #fff0d9; */
            background: var(--card-bg);
            padding: 40px;
            border-radius: 40px;
            width: 90%;
            max-width: 500px;
            text-align: center;
            box-shadow: var(--card-bx-shadow);
            backdrop-filter: blur(10px);
        }

        h2 {
            font-size: 28px;
            margin-bottom: 25px;
        }

        .subtitle {
            color: var(--subtitle-color);
            margin-bottom: 30px;
            font-size: 15px;
        }

        .drop-zone {
            border: var(--drop-zone-border);
            background: var(--drop-zone-bg);
            border-radius: 25px;
            padding: 50px 20px;
            cursor: pointer;
            transition: all 0.3s ease;
            position: relative;
            margin-bottom: 20px;
        }

        .drop-zone:hover,
        .drop-zone.dragover {
            background: var(--drop-zone-dragover-bg);
            transform: scale(1.02);
            border-style: solid;
        }

        .drop-zone p {
            font-weight: bold;
            margin: 0;
            pointer-events: none;
        }

        .drop-zone i {
            display: block;
            font-size: 33px;
            margin-bottom: 10px;
            color: var(--drop-zone-i-color);
        }

        #file {
            display: none;
        }

        button {
            background: white;
            color: rgb(34, 197, 94, 0.9);
            font-weight: bold;
            border: none;
            padding: 13px 35px;
            border: 2px solid rgb(34, 197, 94, 0.9);
            border-radius: 50px;
            font-size: 18px;
            cursor: pointer;
            transition: all 0.2s;
            box-shadow: 0 4px 15px rgb(34, 197, 94, 0.9);
            width: 100%;
            text-transform: uppercase;
            letter-spacing: 1px;
        }

        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 20px rgb(34, 197, 94);
            color: white;
            background: rgb(34, 197, 94);
        }

        .progress {
            width: 100%;
            background: var(--progress-bg);
            border-radius: 20px;
            margin-top: 25px;
            height: 12px;
            overflow: hidden;
            display: none;
        }

        .bar {
            height: 100%;
            width: 0%;
            background: var(--bar-bg);
            border-radius: 20px;
            transition: width 0.3s;
        }

        #status {
            font-size: 14px;
            margin-top: 10px;
            font-weight: 600;
        }

        .disconnect {
            margin-top: 20px;
            background: white;
            color: #ff2e3f;
            border: 2px solid #ff2e3f;
            padding: 10px;
            font-size: 14px;
            box-shadow: 0 4px 15px #ff2e3f;
        }

        .disconnect:hover {
            background: #ff2e3f;
            color: white;
            box-shadow: 0 6px 20px #ff2e3f;
        }

        .splash-text-area {
            margin-top: 25px;
            /* color: #00CAFF; */
            /* color: #CE85E4; */
            color: #D4AF37;
            font-size: 18px;
            font-style: italic;
            font-weight: bold;
            letter-spacing: 1px;
            opacity: 0;
            text-shadow: 0 2px 4px rgba(0, 0, 0, 0.3);
            animation: fadeInUp 1.2s ease-out forwards;
            animation-delay: 3s;
        }

        @keyframes fadeInUp {
            from {
                opacity: 0;
                transform: translateY(10px);
            }

            to {
                opacity: 0.9;
                transform: translateY(0);
            }
        }


        /* Fixed to top-right of the screen */
        .toggle-container {
            position: fixed;
            top: 20px;
            right: 20px;
            display: flex;
            align-items: center;
            gap: 12px;
            z-index: 1000;
            /* Glass effect */
            background: rgba(255, 255, 255, 0.15);
            backdrop-filter: blur(10px);
            padding: 10px 16px;
            border-radius: 50px;
            border: 1px solid rgba(255, 255, 255, 0.2);
            transition: all 0.4s ease;
        }

        /* "DARK MODE" Text on the LEFT */
        .toggle-container::before {
            content: "DARK MODE";
            font-size: 11px;
            font-weight: 900;
            letter-spacing: 1px;
            color: white;
            /* Contrast against the background gradients */
            opacity: 0.9;
        }

        /* The Switch Track on the RIGHT */
        .switch-label {
            width: 42px;
            height: 22px;
            background-color: rgba(255, 255, 255, 0.3);
            display: flex;
            align-items: center;
            border-radius: 50px;
            padding: 2px;
            cursor: pointer;
            transition: background-color 0.3s;
        }

        /* The Sliding Ball */
        .switch-ball {
            width: 18px;
            height: 18px;
            background-color: white;
            border-radius: 50%;
            transition: transform 0.3s cubic-bezier(0.4, 0, 0.2, 1);
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
        }

        /* Mode-Specific Changes */
        :root:has(#theme-toggle:checked) .switch-label {
            background-color: #22c55e;
            /* Green for "On" */
        }

        :root:has(#theme-toggle:checked) .switch-ball {
            transform: translateX(20px);
        }

        /* Responsive Mobile Tweak */
        @media (max-width: 480px) {
            .toggle-container {
                top: 15px;
                right: 15px;
                padding: 8px 12px;
            }

            .toggle-container::before {
                font-size: 10px;
            }
        }
    </style>
</head>

<body>
    <div class="toggle-container">
        <input type="checkbox" id="theme-toggle" hidden>
        <label for="theme-toggle" class="switch-label">
            <span class="switch-ball"></span>
        </label>
    </div>
    <div class="card">
        <h2>Music Streamer</h2>
        <p class="subtitle">Upload your audio files here</p>

        <div class="drop-zone" onclick="document.getElementById('file').click()" id="drop-zone">
            <p id="file-name">Drop <i>.wav</i> or Click to Browse</p>
            <input type="file" id="file" accept=".wav" onchange="updateFileName()">
        </div>

        <button onclick="upload()">Upload Now</button>

        <div class="progress" id="progress-container">
            <div class="bar" id="bar"></div>
        </div>
        <p id="status"></p>

        <button class="disconnect" onclick="disconnect()">Disconnect Stream</button>
    </div>
    <div class="splash-text-area">
        <p id="splash-text"></p>
    </div>

    <script>
        const dropZone = document.getElementById('drop-zone');
        const fileInput = document.getElementById('file');
        const texts = ["Acceptance", "Patience", "Patience"];

        let text = `${texts[Math.floor(Math.random() * texts.length)]} is a great virtue.`;
        document.getElementById('splash-text').innerHTML = text;

        function updateFileName() {
            const name = fileInput.files[0] ? fileInput.files[0].name : "Drop your .WAV here or Click";
            document.getElementById('file-name').innerText = name;
        }

        ['dragover', 'dragleave', 'drop'].forEach(evt => {
            dropZone.addEventListener(evt, e => {
                e.preventDefault();
                if (evt === 'dragover') dropZone.classList.add('dragover');
                else dropZone.classList.remove('dragover');
            });
        });

        dropZone.addEventListener('drop', e => {
            fileInput.files = e.dataTransfer.files;
            updateFileName();
        });

        function upload() {
            let file = fileInput.files[0];
            if (!file) {
                alert("Please select a file first!");
                return;
            }

            document.getElementById("progress-container").style.display = "block";
            let xhr = new XMLHttpRequest();
            let formData = new FormData();
            formData.append("data", file);

            xhr.upload.onprogress = function (e) {
                if (e.lengthComputable) {
                    let p = (e.loaded / e.total) * 100;
                    document.getElementById("bar").style.width = p + "%";
                    document.getElementById("status").innerText = "Sending magic... " + p.toFixed(1) + "%";
                }
            };

            xhr.onload = function () {
                document.getElementById("status").innerText = "Upload complete!";
                setTimeout(() => {
                    document.getElementById("progress-container").style.display = "none";
                    document.getElementById("bar").style.width = "0%";
                }, 2000);
            };

            xhr.open("POST", "/upload", true);
            xhr.send(formData);
        }

        function disconnect() {
            document.getElementById('status').innerText = 'Disconnected';
            fileInput.value = "";
            fetch("/disconnect");
        }
    </script>
</body>

</html>
)rawliteral";
}



static void showFiles(void){
    Serial.printf("\n\n\n");
    File f = SD.open("/");
    while(1){
        File entry = f.openNextFile();
        if(!entry) break;
        Serial.println(entry.name());
        entry.close();
    }
    Serial.printf("\n\n\n");
}



bool downloadFile(const char *ssid, const char *password){
    showFiles();
    Serial.printf("Entering WiFi Upload Mode...\n");

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    WiFi.setSleep(false);
    WiFi.setTxPower(WIFI_POWER_19_5dBm);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("Connect to: http://");
    Serial.println(IP);

    File uploadFile;
    String filename;
    uint32_t bufIndex = 0;

    bool exitUploadMode = false;
    bool uploadError = false;

    server.on("/", HTTP_GET, [](){
        server.send(200, "text/html", getWebpage());
    });

    server.on("/disconnect", HTTP_GET, [&](){
        server.send(200, "text/plain", "Disconnecting...");
        exitUploadMode = true;
    });

    server.on("/upload", HTTP_POST, [&](){
        if(uploadError){
            server.send(500, "text/plain", "Upload failed!");
        }else{
            server.send(200, "text/plain", "Upload Complete");
        }
    },
    [&](){
        HTTPUpload& upload = server.upload();
        if(upload.status == UPLOAD_FILE_START){
            
            bufIndex = 0;
            filename = "/" + upload.filename;

            if(SD.exists(filename))
                SD.remove(filename);
            
            uploadFile = SD.open(filename, FILE_WRITE);
            if(!uploadFile){
                uploadError = true;
                Serial.println("SD open failed!");
            }
        
        }else if(upload.status == UPLOAD_FILE_WRITE){
            
            if(uploadFile){
                uint32_t remaining = upload.currentSize;
                uint32_t offset = 0;
                
                while(remaining > 0){
                    uint32_t space = sizeof(sdUploadBuf) - bufIndex;
                    uint32_t toCopy = remaining;

                    if(toCopy > space)
                        toCopy = space;

                    memcpy(sdUploadBuf + bufIndex, upload.buf + offset, toCopy);

                    bufIndex += toCopy;
                    offset += toCopy;
                    remaining -= toCopy;

                    if(bufIndex == sizeof(sdUploadBuf)){
                        uploadFile.write(sdUploadBuf, bufIndex);
                        bufIndex = 0;
                    }
                }
            }
        }else if(upload.status == UPLOAD_FILE_END){
            if(uploadFile){
                if(bufIndex > 0){
                    uploadFile.write(sdUploadBuf, bufIndex);
                    bufIndex = 0;
                }
                uploadFile.close();
                Serial.println("Upload completed");
            }
        
        }else if(upload.status == UPLOAD_FILE_ABORTED){
            uploadError = true;
            Serial.println("Upload aborted!");
            if(uploadFile)          uploadFile.close();
            if(SD.exists(filename)) SD.remove(filename);
        }
    });

    server.enableDelay(false);
    server.begin();

    while(!exitUploadMode && !uploadError){
        server.handleClient();
        delay(0); // NEED FOR SPEED
    }

    server.close();
    WiFi.mode(WIFI_OFF);
    delay(100); // important for stability
    Serial.printf("Exitting WiFi Upload Mode...");
    showFiles();
    return uploadError;
}