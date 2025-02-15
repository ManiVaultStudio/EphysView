
function drawTrace(jsonDoc)
{
    log("Draw trace");
    
    d3.select("svg").remove();

    // Set dimensions and margins
    var margin = { top: 5, right: 15, bottom: 20, left: 30 },
        width = 200 - margin.left - margin.right,
        height = 180 - margin.top - margin.bottom;

    function drawGraph(selector, jsonDoc)
    {
        // Append the SVG object to the container
        var svg = d3.select(selector)
            .append("svg")
            .attr("width", width + margin.left + margin.right)
            .attr("height", height + margin.top + margin.bottom)
            .append("g")
            .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

        // Extract the data
        let xData = jsonDoc.xData;
        let yData = jsonDoc.yData;

        // Convert to D3-friendly format
        let data = xData.map((x, i) => ({ x: x, y: yData[i] }));

        // Define scales
        var x = d3.scaleLinear()
            .domain(d3.extent(data, d => d.x))
            .range([0, width]);

        var y = d3.scaleLinear()
            .domain([d3.min(data, d => d.y), d3.max(data, d => d.y)])
            .range([height, 0]);

        // Add X axis
        svg.append("g")
            .attr("transform", "translate(0," + height + ")")
            .call(d3.axisBottom(x).ticks(3));

        // Add Y axis
        svg.append("g")
            .call(d3.axisLeft(y));

        // Add the line
        svg.append("path")
            .datum(data)
            .attr("fill", "none")
            .attr("stroke", "steelblue")
            .attr("stroke-width", 1.5)
            .attr("d", d3.line()
                .x(d => x(d.x))
                .y(d => y(d.y))
            );
    }
    
    var keyCount  = Object.keys(jsonDoc).length;
    
    var div = document.getElementById("traceRow");
    // Clear column divs
    while (div.firstChild)
    {
        div.removeChild(div.lastChild);
    }
    // Generate new ones
    for (let i = 0; i < keyCount; i++)
    {
        var newDiv = document.createElement('div');
        newDiv.className = "col-md-1";
        newDiv.setAttribute("id", "container" + i);
        div.appendChild(newDiv);
        
        drawGraph("#container" + i, jsonDoc["data_"+i])
    }
}
