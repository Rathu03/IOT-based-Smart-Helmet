import sys
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression
import seaborn as sns

def main():
    # Read the CSV file
    data = pd.read_csv('sensor_data_duplicate.csv')
    
    # Perform Linear Regression
    X = data[['mq4']]
    y = data['mq7']
    
    model = LinearRegression()
    model.fit(X, y)
    
    # Print the coefficient and intercept
    coef = model.coef_
    intercept = model.intercept_
    print(f'Coefficients: {coef}')
    print(f'Intercept: {intercept}')
    
    plt.scatter(X,y, color = "m", marker = "o", s = 30) 
    y_pred = coef*X + intercept
    plt.xlabel("MQ-4")
    plt.ylabel("MQ-7")
    plt.plot(X, y_pred, color = "g")
    plt.savefig('../frontend/src/plots/linear_regression.png')
    print('linear_regression.png saved')

    dataplot = sns.heatmap(data.corr(), cmap="YlGnBu", annot=True) 
    dataplot.figure.savefig('../frontend/src/plots/dataplot.png')
    print('dataplot.png saved')

    sns.pairplot(data, kind='reg')
    plt.savefig('../frontend/src/plots/pairplot.png')
    print('pairplot.png saved')

if __name__ == "__main__":
    main()
