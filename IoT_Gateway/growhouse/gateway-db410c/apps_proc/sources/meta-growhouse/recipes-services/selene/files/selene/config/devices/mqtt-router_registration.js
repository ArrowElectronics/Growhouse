function transpose(incoming_payload)
{
    var deviceRegistrationPayload = JSON.parse(incoming_payload);
    var transposedObj = [];

    for(var i=0;i<deviceRegistrationPayload.properties.length;i++)
    {
	    deviceRegistrationPayload.properties[i].operation = "";
	    if(deviceRegistrationPayload.properties[i].readOnly==false && deviceRegistrationPayload.properties[i].writeOnly==false)
	    {
		    deviceRegistrationPayload.properties[i].operation = "rw";
		    delete deviceRegistrationPayload.properties[i].readOnly
			    delete deviceRegistrationPayload.properties[i].writeOnly
	    }
    }
    transposedObj.push
	    (
	     {
	     'deviceName':deviceRegistrationPayload.deviceName,
	     'deviceUid': deviceRegistrationPayload.deviceUid,
	     "deviceType":  deviceRegistrationPayload.deviceType,
	     "metadata":
	     {
	     "nodeType":deviceRegistrationPayload.nodeType,
	     "manufacturer": deviceRegistrationPayload.manufacturer,
	     "softwareVersion" : deviceRegistrationPayload.softwareVersion,
	     "softwareName":deviceRegistrationPayload.softwareName,
	     "led1":deviceRegistrationPayload.led1,
	     "led2":deviceRegistrationPayload.led2,
	     "led3":deviceRegistrationPayload.led3,
	     "led4":deviceRegistrationPayload.led4,
	     "led5":deviceRegistrationPayload.led5,
	     "led6":deviceRegistrationPayload.led6
	     },
	     'properties':deviceRegistrationPayload.properties,
	     "softwareVersion" : deviceRegistrationPayload.softwareVersion,
	     "softwareName":deviceRegistrationPayload.softwareName
	     }
	    )
	    transposed_payload = JSON.stringify(transposedObj);
    return transposed_payload;
}
