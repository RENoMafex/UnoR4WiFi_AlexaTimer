# Steps
## 1. Replace
### AlexaTimerPull.xml
`<!-- device id of the alexa you want to use -->` should be replaced with the device id, which you can aquire in your ioBroker dashboard by clicking "objects" in the left sidebar. Then double click on "alexa2", "0", "Echo-Devices". There you will see the device IDs.
## 2. Import
From your ioBroker dashboard, click "scipts" in the left sidebar, than click on the three dots and select "import scripts", than just drag-and-drop `AlexaTimerPull.xml`, `convert.js` and `remainder2mqtt.xml` onto the designated drag-and-dropping-area.
## 3. Activate
Than just activate the three scripts, usually they populate the needed objects by themself!