function transpose(deviceUid,deviceName,deviceStates){
    var transposedObj = {};
    transposedObj['command'] = 'set_device_state';
    transposedObj['deviceUid'] = deviceUid;

    transposedObj['properties'] = {};
    deviceStatesInput = JSON.parse(deviceStates)
    for (var i = 0; i < deviceStatesInput.length; i++) {

                var   propertyValue   =   deviceStatesInput[i].propertyValue   ===   'true'   ?   true   :
(deviceStatesInput[i].propertyValue === 'false' ? false : deviceStatesInput[i].propertyValue)

        if (propertyValue != true && propertyValue != false) {
            if (!isNaN(deviceStatesInput[i].propertyValue)) {
                propertyValue = Number(deviceStatesInput[i].propertyValue)
            }
        }else{
                deviceStatesInput[i].propertyValue= propertyValue
        }

        transposedObj['properties'][deviceStatesInput[i].propertyName] = deviceStatesInput[i].propertyValue

    }
    return JSON.stringify(transposedObj)
}