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

        var data = create_data('Temperature');
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

    var data = create_data(chart_parameter_select.value);
    var layout = {
        title: {
            text: chart_parameter_select.value,
        },
    };

    Plotly.react(chart, data, layout);
}

function create_data(parameter){
    const axis = get_axis(parameter);

    const avg_data = {
        x: axis["X"],
        y: axis["avg_y"],
        name: 'Average',
        mode: 'lines',
        line: {
            color: 'rgb(0, 255, 0)',
        },
    };
    const min_data = {
        x: axis["X"],
        y: axis["min_y"],
        name: 'Minimum',
        mode: 'lines',
        line: {
            color: 'rgb(0, 0, 255)',
            dash: 'dot',
        },
    };
    const max_data = {
        x: axis["X"],
        y: axis["max_y"],
        name: 'Maximum',
        mode: 'lines',
        line: {
            color: 'rgb(255, 0, 0)',
            dash: 'dot',
        },
    };

    return [avg_data, min_data, max_data];
}

function get_axis(parameter){
    var X = [];
    var avg_y = [];
    var min_y = [];
    var max_y = [];

    switch (parameter){
        case 'Temperature':
            for (let i = 0; i < avg.length; i++){
                X.push(avg[i].day || avg[i].hour);
                avg_y.push(avg[i].temp_avg)
                min_y.push(min_max[i].temp_min)
                max_y.push(min_max[i].temp_max)
            }
            break;

        case 'Humidity':
            for (let i = 0; i < avg.length; i++){
                X.push(avg[i].day || avg[i].hour);
                avg_y.push(avg[i].hum_avg)
                min_y.push(min_max[i].hum_min)
                max_y.push(min_max[i].hum_max)
            }
            break;

        case 'Soil Moisture':
            for (let i = 0; i < avg.length; i++){
                X.push(avg[i].day || avg[i].hour);
                avg_y.push(avg[i].moist_avg)
                min_y.push(min_max[i].moist_min)
                max_y.push(min_max[i].moist_max)
            }
            break;

        case 'CO2':
            for (let i = 0; i < avg.length; i++){
                X.push(avg[i].day || avg[i].hour);
                avg_y.push(avg[i].co2_avg)
                min_y.push(min_max[i].co2_min)
                max_y.push(min_max[i].co2_max)
            }
            break;
    }

    return {
        "X": X,
        "avg_y": avg_y,
        "min_y": min_y,
        "max_y": max_y,
    };
}
