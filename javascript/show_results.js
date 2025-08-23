function UrlExists(url)
{
    try
    {
        var http = new XMLHttpRequest();
        http.open('GET', url, false);
        http.send();
        return http.status != 404;
    }
    catch(error)
    {
        return false;
    }
}

function GetMainPath()
{
    return "../../figs/samplers/";
}

function GetPointPath(name)
{
    return [GetMainPath() + name + ".dat", GetMainPath() + name + ".png"]
}

function GetSpectrumPath(name)
{
    return [GetMainPath() + name + "_spectrum.dat", GetMainPath() + name + "_spectrum.png"]
}

function InsertData(name, location, files)
{
    var header = document.createElement("h3");
    header.textContent = name;

    location.appendChild(header);

    if (!UrlExists(files[0]) || !UrlExists(files[1]))
    {
        var info = document.createElement("p");
        info.textContent = "Unavailable";

        location.appendChild(info);
        return;
    }

    var data = document.createElement("a");
    data.textContent = "Raw data";
    data.href = files[0];
    data.style.display = "block";

    var img = document.createElement("img");
    img.src = files[1];

    location.appendChild(data);
    location.appendChild(img);
}

function show_results(override_name)
{
    var path = window.location.pathname;
    var name = "";
    
    if (override_name !== undefined)
    {
        name = override_name;   
    }
    else
    {
        name = path.split('/').slice(-2, -1)[0];
    }
    
    var location = document.getElementsByClassName('results')[0];

    var pts_path = GetPointPath(name);
    var spe_path = GetSpectrumPath(name);

    InsertData("Points", location, pts_path);
    InsertData("Spectrum", location, spe_path);
}