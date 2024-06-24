function transpose(incoming_payload) {
    var transposed_payload = [];
       var incomingObj = JSON.parse(incoming_payload);
    transposed_payload.push(incomingObj)
       return JSON.stringify(transposed_payload);
}
