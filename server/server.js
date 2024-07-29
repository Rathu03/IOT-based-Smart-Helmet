const { SerialPort } = require('serialport');
const cors = require('cors');
const express = require('express');
const fs = require('fs');
const path = require('path');
const { spawn } = require('child_process');

const app = express();
app.use(cors());
const port1 = 5000;

let result;

// Path to the CSV file
const csvFilePath = './sensor_data.csv'
const duplicateFilePath = './sensor_data_duplicate.csv'

// Initialize the CSV file with headers if it doesn't exist
if (!fs.existsSync(csvFilePath)) {
    fs.writeFileSync(csvFilePath, 'mq4,mq7,temperature,humidity\n', 'utf8');
}

// Serve the result via API endpoint
app.get('/result', (req, res) => {
    res.json(result);
});

app.post('/analyze', (req, res) => {
    // Ensure the duplicate file doesn't exist or overwrite it
    fs.copyFileSync(csvFilePath, duplicateFilePath);

    const pythonProcess = spawn('/usr/bin/python3', ['analyze.py', duplicateFilePath]);

    let analysisResult = '';

    pythonProcess.stdout.on('data', (data) => {
        analysisResult += data.toString();
    });

    pythonProcess.stderr.on('data', (data) => {
        console.error(`Error from Python: ${data}`);
    });

    pythonProcess.on('close', (code) => {
        console.log(`Python process exited with code ${code}`);
        res.json({ message: 'Analysis completed', analysisResult });
    });
});

app.listen(port1, () => {
    console.log(`Server listening on port ${port1}`);
});

// Serial port configuration
const config = {
    path: '/dev/ttyUSB0',
    baudRate: 9600,
    dataBits: 8,
    parity: 'none',
    stopBits: 1,
    autoOpen: true
};

const port = new SerialPort(config);
port.open((err) => {
    if (err) {
        console.log("Error opening the port: " + err.message);
    }
});

port.on('data', (data) => {
    const textData = data.toString();
    const splitData = textData.split(',');
    const obj = {
        mq4: splitData[1],
        mq7: splitData[2],
        temperature: splitData[3],
        humidity: splitData[4]
    };
    result = obj;
    console.log(obj);

    let target = 0;
    if(obj.mq4 < 500 && obj.mq7 < 500 && (obj.humidity >= 30 && obj.humidity <= 70) && (obj.temperature >=25 && obj.temperature < 50)) {
        target = 1;
    }
    const csvLine = `${obj.mq4},${obj.mq7},${obj.temperature},${obj.humidity},${target}\n`;
    fs.appendFile(csvFilePath, csvLine, (err) => {
        if (err) {
            console.error('Error writing to CSV file', err);
        }
    });
});
