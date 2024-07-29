// src/App.js

import React, { useState, useEffect } from 'react';
import linear_regression from './plots/linear_regression.png'
import pairplot from './plots/pairplot.png'
import dataplot from './plots/dataplot.png'

const App = () => {
  const [data, setData] = useState(null);
  const [analysisResult, setAnalysisResult] = useState(null);

  const getData = async () => {
    try {
      const response = await fetch('http://localhost:5000/result');
      if (!response.ok) {
        throw new Error('Network response was not ok ' + response.statusText);
      }
      const data = await response.json();
      console.log(data);
      setData(data);
    } catch (error) {
      console.error('Fetch error: ', error);
      alert('Failed to fetch data. Please try again later.');
    }
  };
useEffect(() => {
  getData();
  const interval = setInterval(getData, 2000);
  return () => clearInterval(interval);
}, []);

const handleAnalyze = async () => {
  try {
      const response = await fetch('http://localhost:5000/analyze', {
          method: 'POST',
      });
      if (!response.ok) {
          throw new Error('Network response was not ok');
      }
      const result = await response.json();
      setAnalysisResult(result.analysisResult);
      //setImageSrc('pairplot.png'); // Image source for the regression plot
  } catch (error) {
      console.error('Error analyzing data:', error);
  }
};


  return (
    <div className='main-container'>
      <h1 style={{textAlign:"center"}}>Smart Mining Helmet</h1>
      <div>
        <button onClick={getData} style={{ display: 'none' }}>Submit</button>
        <h1>Sensor Data</h1>
        {data ? (
          <div className='data-container'>
            <p>MQ4: {data.mq4}</p>
            <p>MQ7: {data.mq7}</p>
            <p>Temperature: {data.temperature}</p>
            <p>Humidity: {data.humidity}</p>
          </div>
        ) : (
          <p>Loading...</p>
        )}

        <button onClick={handleAnalyze} className='submit-button'>Analyze</button>
        

        {analysisResult && (
          <div>
            <h2>Analysis Result</h2>
            <pre>{analysisResult}</pre>
            <img src={linear_regression} alt="linear_regression" width={1000} height={1000} />
            <img src={pairplot} alt="pairplot" width={1000} height={1000} />
            <img src={dataplot} alt="dataplot" width={1000} height={1000} />
          </div>
        )}
      </div>
    </div>
  );
};

export default App;


