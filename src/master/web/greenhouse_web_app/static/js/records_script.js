const csrftoken = document.querySelector('[name=csrfmiddlewaretoken]').value;

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

