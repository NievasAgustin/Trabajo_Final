//var sheet_id = "11je-SPu0kRXFDXaUOxfZmdXHGbyoVXVdww7FdaKMkqA";
//var sheet_name = "ESP32";
//function doGet(e){
//var ss = SpreadsheetApp.openById(sheet_id);
//var sheet = ss.getSheetByName(sheet_name);
//var date = Number(e.parameter.date);
//var sensor = Number(e.parameter.sensor);
//var value3 = Number(e.parameter.value3);
//sheet.appendRow([date,sensor,value3]);
//}

function doGet(e) {

    let id = '11je-SPu0kRXFDXaUOxfZmdXHGbyoVXVdww7FdaKMkqA'; 
    let sheetName = 'ESP32'; 
    var result;

    // e.parameter has received GET parameters, i.e. temperature, humidity, illumination
    if (e.parameter == undefined) {
        result = 'Parameter undefined';
    } else {
        var sheet = SpreadsheetApp.openById(id).getSheetByName(sheetName);
        var newRow = sheet.getLastRow() + 1;  // get row number to be inserted
        var rowData = [];

        // get current time
        rowData[0] = 
        rowData[1] = e.parameter.tiempo;;
        rowData[1] = e.parameter.outpotencia;
        rowData[2] = e.parameter.dutycycle;
        rowData[3] = e.parameter.dutycyclefinal;
        rowData[4] = e.parameter.outtension;

        // 1 x rowData.length cells from (newRow, 1) cell are specified
        var newRange = sheet.getRange(newRow, 1, 1, rowData.length);

        // insert data to the target cells
        newRange.setValues([rowData]);
        result =  'Ok';
    }

    return ContentService.createTextOutput(result);
}