
function drawTrace(jsonDoc)
{
    const startTime = performance.now()
    log("Draw trace");
    
    d3.select("svg").remove();
    const s1Time = performance.now()

    // Extract the data
    const keyAcq = "acquisition";
    const keyStim = "stimulus";
    const keyTitle = "title";

    function drawStimulus(selector, graphObj)
    {
        // Set dimensions and margins
        var margin = { top: 40, right: 10, bottom: 20, left: 45 },
            width = 200 - margin.left - margin.right,
            height = 120 - margin.top - margin.bottom;
        
        let stim_xData = graphObj[keyStim]["xData"];
        let stim_yData = graphObj[keyStim]["yData"];
        let titleData = graphObj[keyTitle];
        
        let stim_data = stim_xData.map((x, i) => ({ x: x, y: stim_yData[i] }));
        
        // Append the SVG object to the container
        var svg = d3.select(selector)
            .append("svg")
            .attr("width", width + margin.left + margin.right)
            .attr("height", height + margin.top + margin.bottom)
            .style("display", "block")
            .append("g")
            .attr("transform", "translate(" + margin.left + "," + margin.top + ")");
        
        // Add chart title
        svg.append("text")
            .attr("x", width / 2)
            .attr("y", -margin.top / 2)
            .attr("text-anchor", "middle")
            .style("font-size", "12px")
            .style("font-weight", "bold")
            .text(titleData || ""); // default to empty string if title not provided

        // Define scales
        var x = d3.scaleLinear()
            .domain(d3.extent(stim_data, d => d.x))
            .range([0, width]);

        var y = d3.scaleLinear()
            .domain(d3.extent(stim_data, d => d.y * 2))
            .range([height, 0]);

        // Add X axis
        svg.append("g")
            .attr("transform", "translate(0," + height + ")")
            .call(d3.axisBottom(x).ticks(3));

        // Add Y axis
        svg.append("g")
            .call(d3.axisLeft(y).ticks(4));
        
        // Add X axis label
        svg.append("text")
            .attr("x", width / 2) // Center the label
            .attr("y", height + margin.bottom - 10) // Position below the X-axis
            .style("text-anchor", "middle") // Center the text
            .style("font-size", "10px") // Make text smaller
            .text(""); // Unit
    
        // Add Y axis label
        svg.append("text")
            .attr("transform", "rotate(-90)") // Rotate the text for Y-axis
            .attr("y", -margin.left + 5) // Adjust positioning
            .attr("x", -height / 2) // Center the label
            .attr("dy", "1em") // Fine-tune spacing
            .style("text-anchor", "middle") // Center align text
            .style("font-size", "10px") // Make text smaller
            .text("pA"); // Unit

        // Add the stimulus line
        svg.append("path")
            .datum(stim_data)
            .attr("fill", "none")
            .attr("stroke", "orangered")
            .attr("stroke-width", 1.5)
            .attr("d", d3.line()
                .x(d => x(d.x))
                .y(d => y(d.y))
            );
    }

    function drawGraph(selector, graphObj)
    {
        // Set dimensions and margins
        var margin = { top: 20, right: 10, bottom: 40, left: 45 },
            width = 200 - margin.left - margin.right,
            height = 180 - margin.top - margin.bottom;
        
        const ps = performance.now()
        
        // Append the SVG object to the container
        var svg = d3.select(selector)
            .append("svg")
            .attr("width", width + margin.left + margin.right)
            .attr("height", height + margin.top + margin.bottom)
            .style("display", "block")
            .append("g")
            .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

        const pm1 = performance.now()

        // Avoid undefined access
        if (!graphObj[keyAcq] || !graphObj[keyStim]) {
            log(`Missing data for index ${i}:`, graphObj);
            return;
        }

        let acq_xData = graphObj[keyAcq]["xData"];
        let acq_yData = graphObj[keyAcq]["yData"];

        let stim_xData = graphObj[keyStim]["xData"];
        let stim_yData = graphObj[keyStim]["yData"];
        let titleData = graphObj[keyTitle];
        
        const pm2 = performance.now()
        // Convert to D3-friendly format
        let acq_data = acq_xData.map((x, i) => ({ x: x, y: acq_yData[i] }));
        let stim_data = stim_xData.map((x, i) => ({ x: x, y: stim_yData[i] }));

        const pm3 = performance.now()
        
        // // Add chart title
        // svg.append("text")
            // .attr("x", width / 2)
            // .attr("y", -margin.top / 2)
            // .attr("text-anchor", "middle")
            // .style("font-size", "12px")
            // .style("font-weight", "bold")
            // .text(titleData || ""); // default to empty string if title not provided
        
        // Define scales
        var x = d3.scaleLinear()
            .domain(d3.extent(acq_data, d => d.x))
            .range([0, width]);

        var y = d3.scaleLinear()
            .domain([-120, 50])
            .range([height, 0]);

        const pm4 = performance.now()
        // Add X axis
        svg.append("g")
            .attr("transform", "translate(0," + height + ")")
            .call(d3.axisBottom(x).ticks(3));

        // Add Y axis
        svg.append("g")
            .call(d3.axisLeft(y));

        const pm5 = performance.now()
        // Add X axis label
        svg.append("text")
            .attr("x", width / 2) // Center the label
            .attr("y", height + margin.bottom - 10) // Position below the X-axis
            .style("text-anchor", "middle") // Center the text
            .style("font-size", "10px") // Make text smaller
            .text("ms"); // Unit
    
        // Add Y axis label
        svg.append("text")
            .attr("transform", "rotate(-90)") // Rotate the text for Y-axis
            .attr("y", -margin.left + 5) // Adjust positioning
            .attr("x", -height / 2) // Center the label
            .attr("dy", "1em") // Fine-tune spacing
            .style("text-anchor", "middle") // Center align text
            .style("font-size", "10px") // Make text smaller
            .text("mV"); // Unit

        const pm6 = performance.now()
        // Add the acquisition line
        svg.append("path")
            .datum(acq_data)
            .attr("fill", "none")
            .attr("stroke", "steelblue")
            .attr("stroke-width", 1.5)
            .attr("d", d3.line()
                .x(d => x(d.x))
                .y(d => y(d.y))
            );
        
        // // Add the stimulus line
        // svg.append("path")
            // .datum(stim_data)
            // .attr("fill", "none")
            // .attr("stroke", "orangered")
            // .attr("stroke-width", 1.5)
            // .attr("d", d3.line()
                // .x(d => x(d.x))
                // .y(d => y(d.y))
            // );
            
        const pe = performance.now()
        
        log(`Call to drawGraph took ${pe - pm6} ${pm6 - pm5} ${pm5 - pm4} ${pm4 - pm3} ${pm3 - pm2} ${pm2 - pm1}  ${pm1 - ps} milliseconds`)
    }
    const s2Time = performance.now()
    
    let graphArray = jsonDoc["graphs"]
    var numGraphs  = graphArray.length;
    
    var div = document.getElementById("traceRow");
    // Clear column divs
    while (div.firstChild)
    {
        div.removeChild(div.lastChild);
    }
        const s3Time = performance.now()
    // Generate new ones
    for (let i = 0; i < numGraphs; i++)
    {
        var newDiv = document.createElement('div');
        newDiv.className = "chart-div"
        newDiv.setAttribute("id", "container" + i);
        div.appendChild(newDiv);

        drawStimulus("#container" + i, graphArray[i])
        drawGraph("#container" + i, graphArray[i])
    }
    const endTime = performance.now()
    
    log(`Call to drawTrace took ${endTime - s3Time} ${s3Time - s2Time} ${s2Time - s1Time} ${s1Time - startTime} milliseconds`)
}
