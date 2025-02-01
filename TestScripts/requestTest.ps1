# Define the URL to shorten
$longURL = "https://www.youtube.com/watch?v=-twin8RbOeM"

# Define the JSON body
$jsonBody = @{
    url = $longURL
} | ConvertTo-Json

# Send the POST request to the /shorten endpoint
$response = Invoke-WebRequest -Uri "http://127.0.0.1:18080/shorten" -Method POST -Body $jsonBody -ContentType "application/json"

# Display the response
Write-Host "Shortened URL Response:"
$response.Content