document.getElementById("form_input_options").addEventListener("change", function (e){
    const new_input = document.createElement("input");

    switch (this.value){
        case "date":
            new_input.type = "date";
            break;
        default:
            new_input.type = "number";
            break;
    }

    new_input.name = "search_input";
    new_input.id = "search_input";
    new_input.required = true;

    document.getElementById("search_input").replaceWith(new_input);
});

document.getElementById("display_chart").addEventListener("change", function (e){
    const chart_div = document.getElementById("chart_div");

    if (this.checked){
        chart_div.style.display = 'block';

        const chart = document.createElement("div");
        chart.id = "chart";

        const chart_parameter_select = document.createElement("select");
        chart_parameter_select.id = "char_var_select";
        
        const parameter_names = ["Temperature", "Humidity", "Soil Moisture", "CO2"];
        for (const parameter_name of parameter_names){
            const option = document.createElement("option");
            option.value = parameter_name;
            option.innerHTML = parameter_name;

            chart_parameter_select.appendChild(option);
        }

        chart_parameter_select.addEventListener("change", option_listener);

        var X = [];
        var avg_y = [];
        var min_y = [];
        var max_y = [];
        for (let i = 0; i < avg.length; i++){
            X.push(avg[i].day);
            avg_y.push(avg[i].temp_avg)
            min_y.push(min_max[i].temp_min)
            max_y.push(min_max[i].temp_max)
        }

        var data = create_data(X, avg_y, min_y, max_y);
        var layout = {
            title: {
                text: 'Temperature',
            },
        };

        chart_div.appendChild(chart_parameter_select);
        chart_div.appendChild(chart);

        Plotly.newPlot(chart, data, layout);
    }else{
        chart_div.style.display = 'none';
        chart_div.innerHTML = '';
    }
});

function option_listener(e) {
    const chart_parameter_select = document.getElementById("char_var_select");
    const chart = document.getElementById("chart");

    var X = [];
    var avg_y = [];
    var min_y = [];
    var max_y = [];

    switch (chart_parameter_select.value){
        case 'Temperature':
            for (let i = 0; i < avg.length; i++){
                X.push(avg[i].day);
                avg_y.push(avg[i].temp_avg)
                min_y.push(min_max[i].temp_min)
                max_y.push(min_max[i].temp_max)
            }
            break;

        case 'Humidity':
            for (let i = 0; i < avg.length; i++){
                X.push(avg[i].day);
                avg_y.push(avg[i].hum_avg)
                min_y.push(min_max[i].hum_min)
                max_y.push(min_max[i].hum_max)
            }
            break;

        case 'Soil Moisture':
            for (let i = 0; i < avg.length; i++){
                X.push(avg[i].day);
                avg_y.push(avg[i].moist_avg)
                min_y.push(min_max[i].moist_min)
                max_y.push(min_max[i].moist_max)
            }
            break;

        case 'CO2':
            for (let i = 0; i < avg.length; i++){
                X.push(avg[i].day);
                avg_y.push(avg[i].co2_avg)
                min_y.push(min_max[i].co2_min)
                max_y.push(min_max[i].co2_max)
            }
            break;
    }

    var data = create_data(X, avg_y, min_y, max_y);
    var layout = {
        title: {
            text: this.value,
        },
    };

    Plotly.react(chart, data, layout);
}

function create_data(X, avg_y, min_y, max_y){
    const avg_data = {
        x: X,
        y: avg_y,
        name: 'Average',
        mode: 'lines',
        line: {
            color: 'rgb(0, 255, 0)',
        },
    };
    const min_data = {
        x: X,
        y: min_y,
        name: 'Minimum',
        mode: 'line',
        line: {
            color: 'rgb(0, 0, 255)',
            dash: 'dot',
        },
    };
    const max_data = {
        x: X,
        y: max_y,
        name: 'Maximum',
        mode: 'line',
        line: {
            color: 'rgb(255, 0, 0)',
            dash: 'dot',
        },
    };

    return [avg_data, min_data, max_data];
}
