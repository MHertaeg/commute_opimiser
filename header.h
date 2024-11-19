
#ifndef HEADER_H
#define HEADER_H
#include <wx/wxprec.h>
#include <wx/dcbuffer.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <iostream>
#include <thread>  // For std::this_thread::sleep_for
#include <chrono>  // For std::chrono::seconds


struct coordinate_input {
    double latitude;
    double longitude;
    double time;
};

struct pixel_data {
    double value;
    double colour;

};

struct GPSCoordinate {
    double latitude;
    double longitude;
};

struct PixelCoordinate {
    int x;
    int y;
};

struct MapBounds {
    GPSCoordinate topLeft;
    GPSCoordinate topRight;
    GPSCoordinate bottomLeft;
    GPSCoordinate bottomRight;
};
struct input_data_struct {
    std::vector<coordinate_input> input_data_csl_max;
    std::vector<coordinate_input> input_data_csl_min;
    std::vector<coordinate_input> input_data_TT_max;
    std::vector<coordinate_input> input_data_TT_min;
};

double linear_interpolate(const coordinate_input& p1, const coordinate_input& p2, double latitude, double longitude) {
    // Ensure latitude and longitude of the points are different to avoid division by zero
    if (p1.latitude == p2.latitude && p1.longitude == p2.longitude) {
        throw std::invalid_argument("Coordinates p1 and p2 cannot be identical.");
    }

    // Calculate the distance between the two points
    double dist1 = std::sqrt(std::pow(p1.latitude - latitude, 2) + std::pow(p1.longitude - longitude, 2));
    double dist2 = std::sqrt(std::pow(p2.latitude - latitude, 2) + std::pow(p2.longitude - longitude, 2));
    //std::cout << "distance   " << dist1 << " , " << dist2 << std::endl;
    // Linear interpolation formula
    double weight1 = 1.0 / dist1;
    double weight2 = 1.0 / dist2;

    double interpolated_time = (p1.time * weight1 + p2.time * weight2) / (weight1 + weight2);
    return interpolated_time;
}


std::vector<coordinate_input> find_closest_three_points(const std::vector<coordinate_input>& input_data, double latitude, double longitude) {
    // Ensure we have at least three points
    if (input_data.size() < 3) {
        throw std::invalid_argument("At least three points are required for interpolation.");
    }

    // Initialize the closest points with the first three points
    std::vector<coordinate_input> closest_points = { input_data[0], input_data[1], input_data[2] };
    std::vector<double> min_dists = {
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max()
    };

    // Find the three closest points
    for (const auto& point : input_data) {
        double dist = std::sqrt(std::pow(point.latitude - latitude, 2) + std::pow(point.longitude - longitude, 2));

        // Update the closest points if necessary
        if (dist < min_dists[0]) {
            // Shift everything one position
            min_dists[2] = min_dists[1];
            min_dists[1] = min_dists[0];
            min_dists[0] = dist;

            closest_points[2] = closest_points[1];
            closest_points[1] = closest_points[0];
            closest_points[0] = point;
        }
        else if (dist < min_dists[1]) {
            // Shift second to third
            min_dists[2] = min_dists[1];
            min_dists[1] = dist;

            closest_points[2] = closest_points[1];
            closest_points[1] = point;
        }
        else if (dist < min_dists[2]) {
            min_dists[2] = dist;
            closest_points[2] = point;
        }
    }

    /*
    // Debug output
    for (size_t i = 0; i < 3; ++i) {
        std::cout << "Closest Point " << (i+1) << ": Latitude = " << closest_points[i].latitude
                  << ", Longitude = " << closest_points[i].longitude
                  << ", Time = " << closest_points[i].time
                  << ", Distance = " << min_dists[i] << std::endl;
    }
    */

    return closest_points;
};
    double interpolate_closest_three_points(const std::vector<coordinate_input>&input_data, double latitude, double longitude) {
        auto closest_points = find_closest_three_points(input_data, latitude, longitude);

        // Here you can implement your preferred interpolation method using these three points
        // For now, let's use a simple weighted average based on inverse distance
        double total_weight = 0.0;
        double weighted_sum = 0.0;

        for (const auto& point : closest_points) {
            double dist = std::sqrt(std::pow(point.latitude - latitude, 2) +
                std::pow(point.longitude - longitude, 2));
            double weight = 1.0 / (dist + 1e-10); // Adding small number to avoid division by zero
            weighted_sum += point.time * weight;
            total_weight += weight;
        }

        return weighted_sum / total_weight;
    };

    double linear_interpolate_closest_points(const std::vector<coordinate_input>& input_data, double latitude, double longitude) {
        // Ensure we have at least two points to interpolate
        if (input_data.size() < 2) {
            throw std::invalid_argument("At least two points are required for interpolation.");
        }

        // Initialize the closest points with the first two points
        coordinate_input closest_point1 = input_data[0];
        coordinate_input closest_point2 = input_data[1];

        // Initialize distances for the two closest points
        double min_dist1 = std::numeric_limits<double>::max();
        double min_dist2 = std::numeric_limits<double>::max();

        // Find the two closest points
        for (const auto& point : input_data) {
            double dist = std::sqrt(std::pow(point.latitude - latitude, 2) + std::pow(point.longitude - longitude, 2));

            // Check if this point is closer than the first closest
            if (dist < min_dist1) {
                min_dist2 = min_dist1;
                closest_point2 = closest_point1;
                min_dist1 = dist;
                closest_point1 = point;
            }
            // Check if this point is closer than the second closest
            else if (dist < min_dist2) {
                min_dist2 = dist;
                closest_point2 = point;
            }
        }
        /*
        std::cout << "Closest Point 1: Latitude = " << closest_point1.latitude
            << ", Longitude = " << closest_point1.longitude
            << ", Time = " << closest_point1.time << std::endl;

        std::cout << "Closest Point 2: Latitude = " << closest_point2.latitude
            << ", Longitude = " << closest_point2.longitude
            << ", Time = " << closest_point2.time << std::endl;
            */
            // Perform linear interpolation between the two closest points
        return linear_interpolate(closest_point1, closest_point2, latitude, longitude);
    };


double interpolate(std::vector<coordinate_input> input_data, double latitude, double longitude) {
    double dist;
    double weighted_sum = 0.0;
    double weight_total = 0.0;
    const double epsilon = 1e-6;
    for (const auto& point : input_data) {
        dist = std::sqrt((point.latitude - latitude) * (point.latitude - latitude) + (point.longitude - longitude) * (point.longitude - longitude));
        //std::cout << "dist" << " , " << dist << std::endl;
        if (dist < epsilon) { // Handle case when the point is extremely close
            return point.time;
        }

        double weight = 1.0 / dist;
        //std::cout << point.latitude << " , " << point.longitude << " , " << latitude << " , " << longitude << std::endl;
        //std::cout << point.time << " , " << weight << std::endl;
        weighted_sum += point.time * weight;
        weight_total += weight;

    }
    //std::cout << weighted_sum / weight_total << std::endl;
    return (weight_total > 0) ? weighted_sum / weight_total : 0.0;
};


input_data_struct read_csv_full(std::string file_name) {
    input_data_struct input_data;

    int i = 0;
    // Open the CSV file
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file." << std::endl;
        return input_data;
    }

    std::string line;
    // Skip the header line (if present)
    std::getline(file, line);

    // Read each line from the file
    while (std::getline(file, line)) {
        input_data.input_data_csl_max.push_back(coordinate_input{});
        input_data.input_data_csl_min.push_back(coordinate_input{});
        input_data.input_data_TT_max.push_back(coordinate_input{});
        input_data.input_data_TT_min.push_back(coordinate_input{});
        std::stringstream ss(line);
        std::string value;

        double doubleVal;

        // Column 1
        std::getline(ss, value, ',');
        doubleVal = std::stod(value);
        input_data.input_data_csl_max[i].latitude = doubleVal;
        input_data.input_data_csl_min[i].latitude = doubleVal;
        input_data.input_data_TT_max[i].latitude = doubleVal;
        input_data.input_data_TT_min[i].latitude = doubleVal;

        // Column 2
        std::getline(ss, value, ',');
        doubleVal = std::stod(value);
        input_data.input_data_csl_max[i].longitude = doubleVal;
        input_data.input_data_csl_min[i].longitude = doubleVal;
        input_data.input_data_TT_max[i].longitude = doubleVal;
        input_data.input_data_TT_min[i].longitude = doubleVal;

        // Column 3
        std::getline(ss, value, ',');
        doubleVal = std::stod(value);
        input_data.input_data_TT_min[i].time = doubleVal;

        // Column 4
        std::getline(ss, value, ',');
        doubleVal = std::stod(value);
        input_data.input_data_TT_max[i].time = doubleVal;

        // Column 5
        std::getline(ss, value, ',');
        doubleVal = std::stod(value);
        input_data.input_data_csl_min[i].time = doubleVal;

        // Column 6
        std::getline(ss, value, ',');
        doubleVal = std::stod(value);
        input_data.input_data_csl_max[i].time = doubleVal;


        i++;
    }

    file.close();

    return input_data;
};


std::vector<coordinate_input> read_csv(std::string file_name) {
    std::vector<coordinate_input> input_data;
    int i = 0;
    // Open the CSV file
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file." << std::endl;
        return input_data;
    }

    std::string line;
    // Skip the header line (if present)
    std::getline(file, line);

    // Read each line from the file
    while (std::getline(file, line)) {
        input_data.push_back(coordinate_input{});
        std::stringstream ss(line);
        std::string value;

        double doubleVal;

        // Column 1
        std::getline(ss, value, ',');
        doubleVal = std::stod(value);
        input_data[i].latitude = doubleVal;

        // Column 2
        std::getline(ss, value, ',');
        doubleVal = std::stod(value);
        input_data[i].longitude = doubleVal;

        // Column 3
        std::getline(ss, value, ',');
        doubleVal = std::stod(value);
        input_data[i].time = doubleVal;

        i++;
    }

    file.close();

    return input_data;
};


class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};



// Function to convert pixel coordinates to GPS coordinates
GPSCoordinate pixelToGPS(
    const PixelCoordinate& pixel,
    const int imageWidth, const int imageHeight,
    const GPSCoordinate& topLeft, const GPSCoordinate& topRight,
    const GPSCoordinate& bottomLeft, const GPSCoordinate& bottomRight
) {
    // Normalize pixel coordinates to a [0, 1] range
    double x_ratio = static_cast<double>(pixel.x) / imageWidth;
    double y_ratio = static_cast<double>(pixel.y) / imageHeight;

    // Interpolate latitude and longitude
    double lat = topLeft.latitude
        + x_ratio * (topRight.latitude - topLeft.latitude)
        + y_ratio * (bottomLeft.latitude - topLeft.latitude);
    double lon = topLeft.longitude
        + x_ratio * (topRight.longitude - topLeft.longitude)
        + y_ratio * (bottomLeft.longitude - topLeft.longitude);

    return { lat, lon };
}


class MyFrame : public wxFrame
{
public:
    MyFrame(const input_data_struct& input_data);

    MapBounds getMapBounds() const { return mapBounds; }

private:
    input_data_struct m_inputData;
    int m_transparencyMethod = 1;

    MapBounds mapBounds= {
    { -37.673467, 144.900807 }, // topLeft
    { -37.675475, 145.244541 }, // topRight
    { -37.933589, 144.913295 }, // bottomLeft
    { -37.931713, 145.237128 }  // bottomRight
    };


    void OnPaint(wxPaintEvent& event);
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnMouseWheel(wxMouseEvent& event);   // For zooming
    void OnLeftMouseDown(wxMouseEvent& event);    // For initiating panning
    void OnRightMouseDown(wxMouseEvent& event);    // For initiating panning
    void OnMouseMove(wxMouseEvent& event);    // For panning the map
    void OnMouseUp(wxMouseEvent& event);      // For ending panning
    void OnMouseClick(wxMouseEvent& event);   // For outputting coordinates on click
    void PrepareColourmapWithTransparency();
    void SetTransparencyMethod(int method);
    void OnTransparencyMethod1(wxCommandEvent& event);
    void OnTransparencyMethod2(wxCommandEvent& event);
    void OnTransparencyMethod3(wxCommandEvent& event);

    wxBitmap m_backgroundImage;  // Bitmap to store the background image
    wxBitmap m_colourmap;  // Bitmap to store the background image

    double m_zoomLevel = 1.0;                // Zoom factor
    wxPoint m_panOffset = wxPoint(0, 0);     // Offset for panning
    wxPoint m_lastMousePos;                  // Stores the last mouse position for panning

    bool m_isPanning = false;                // Flag for panning mode
};

enum
{
    ID_Hello = 1,
    ID_TRANSPARENCY_METHOD1,
    ID_TRANSPARENCY_METHOD2,
    ID_TRANSPARENCY_METHOD3
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{

    //const std::string filename_input = "C:/Users/michael.h_chemwatch/source/repos/commute/coordinates.csv";
    const std::string filename_input = "coordinates.csv";
    PixelCoordinate data_point;
    GPSCoordinate gps;
    MapBounds mapBounds = {
    { -37.673467, 144.900807 }, // topLeft
    { -37.675475, 145.244541 }, // topRight
    { -37.933589, 144.913295 }, // bottomLeft
    { -37.931713, 145.237128 }  // bottomRight
    };

    int image_width = 1448;
    int image_height = 1340;
    


    //image_data.resize(image_height, std::vector<pixel_data>(image_width));
    //update
    //std::vector<DataPoint> data = readCSV(filename);
    input_data_struct input_data;
    input_data = read_csv_full(filename_input);
    //for (int i = 0; i < input_data.size(); i++)
    //{
    //    std::cout << input_data[i].longitude << std::endl;
    //}

   

    wxImage::AddHandler(new wxPNGHandler);  // Add PNG image handler

    MyFrame* frame = new MyFrame(input_data);
    frame->SetClientSize(800, 600);
    frame->Show(true);
    return true;
}
double find_maximum(std::vector<coordinate_input> input_data_this)
{
    double max_time = 0;
    for (int i = 0; i < input_data_this.size(); i++)
    {
        if (input_data_this[i].time > max_time)
        {
            max_time = input_data_this[i].time;
        };
    };
    return max_time;
};

void MyFrame::PrepareColourmapWithTransparency()
{
    wxImage colourmapImage = m_colourmap.ConvertToImage();
    int image_width = colourmapImage.GetWidth();
    int image_height = colourmapImage.GetHeight();
    GPSCoordinate gps;
    std::vector<coordinate_input> input_data_this;
    double saturation_time = 115;
    if (m_transparencyMethod == 1)
    {
        input_data_this = m_inputData.input_data_csl_min;
    };

    if(m_transparencyMethod == 2)
    {
        input_data_this = m_inputData.input_data_TT_min;
    };

    if (m_transparencyMethod == 3)
    {
        input_data_this.resize(m_inputData.input_data_TT_max.size());
        for (int i = 0; i < m_inputData.input_data_TT_max.size(); i++)
        {
            //std::cout << i << std::endl;
            input_data_this[i].longitude = m_inputData.input_data_TT_min[i].longitude;
            input_data_this[i].latitude = m_inputData.input_data_TT_min[i].latitude;
            input_data_this[i].time = m_inputData.input_data_TT_min[i].time + m_inputData.input_data_csl_min[i].time;
            //std::cout << input_data_this[i].time << std::endl;
        }
        saturation_time = 200;
    };

    //std::cout << input_data_this[0].longitude << std::endl;
    PixelCoordinate data_point;
    std::vector<std::vector<pixel_data>> image_data(image_width, std::vector<pixel_data>(image_height));

    for (int i = 0; i < image_width; ++i) { // Loop over rows
        for (int j = 0; j < image_height; ++j) { // Loop over elements in the row
            data_point = { i, j };
            gps = pixelToGPS(data_point, image_width, image_height, mapBounds.topLeft, mapBounds.topRight, mapBounds.bottomLeft, mapBounds.bottomRight);
            image_data[i][j].value = interpolate_closest_three_points(input_data_this, gps.latitude, gps.longitude);
            if (m_transparencyMethod == 3)
            {
                image_data[i][j].colour = ((image_data[i][j].value - 100) / 50) * 230.0;
            }
            else
            {
                image_data[i][j].colour = ((image_data[i][j].value) / saturation_time) * 230.0;
            };
            if (image_data[i][j].colour > 230.0)
            {
                image_data[i][j].colour = 230;
            };
        }
    }
    
    if (!colourmapImage.HasAlpha())
    {
        colourmapImage.InitAlpha();
    }

    for (int x = 0; x < colourmapImage.GetWidth(); ++x)
    {
        for (int y = 0; y < colourmapImage.GetHeight(); ++y)
        {
            unsigned char red = 255;
            unsigned char green = 0;
            unsigned char blue = 0;
            unsigned char alpha;
            
            // Different transparency methods

            alpha = image_data[x][y].colour;
            
            colourmapImage.SetRGB(x, y, red, green, blue);
            colourmapImage.SetAlpha(x, y, alpha);
        }
    }

    m_colourmap = wxBitmap(colourmapImage);
}

void MyFrame::SetTransparencyMethod(int method)
{
    m_transparencyMethod = method;
    PrepareColourmapWithTransparency();  // Apply the new transparency method

    // Trigger a repaint after setting transparency
    Refresh();
}

void MyFrame::OnTransparencyMethod1(wxCommandEvent& event)
{
    SetTransparencyMethod(1);  // Apply transparency method 1
}

void MyFrame::OnTransparencyMethod2(wxCommandEvent& event)
{
    SetTransparencyMethod(2);  // Apply transparency method 2
}

void MyFrame::OnTransparencyMethod3(wxCommandEvent& event)
{
    SetTransparencyMethod(3);  // Apply transparency method 2
}

MyFrame::MyFrame(const input_data_struct& input_data)
    : wxFrame(NULL, wxID_ANY, "Michael's Commute Optimiser"),
    m_inputData(input_data)
{
    // Initialize mapBounds in the constructor
    
    // Set the background style to wxBG_STYLE_PAINT
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    // Load the background image (change path to your image file)
    //m_backgroundImage.LoadFile("C:/Users/michael.h_chemwatch/source/repos/commute/map.png", wxBITMAP_TYPE_PNG);
    m_backgroundImage.LoadFile("map.png", wxBITMAP_TYPE_PNG);
    m_colourmap = wxBitmap(m_backgroundImage.GetWidth(), m_backgroundImage.GetHeight(), 32);
    PrepareColourmapWithTransparency();
    //m_colourmap.LoadFile("C:/Users/michael.h_chemwatch/source/repos/commute/map2.png", wxBITMAP_TYPE_PNG);

    Bind(wxEVT_PAINT, &MyFrame::OnPaint, this);
    Bind(wxEVT_MOUSEWHEEL, &MyFrame::OnMouseWheel, this);
    Bind(wxEVT_LEFT_DOWN, &MyFrame::OnLeftMouseDown, this);
    Bind(wxEVT_RIGHT_DOWN, &MyFrame::OnRightMouseDown, this);
    Bind(wxEVT_MOTION, &MyFrame::OnMouseMove, this);
    Bind(wxEVT_LEFT_UP, &MyFrame::OnMouseUp, this);

    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
        "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenu* menuTransparency = new wxMenu;
    menuTransparency->Append(ID_TRANSPARENCY_METHOD1, "&Sherilyn's Commute");
    menuTransparency->Append(ID_TRANSPARENCY_METHOD2, "&Michael's Commute");
    menuTransparency->Append(ID_TRANSPARENCY_METHOD3, "&Added");

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuTransparency, "&Visualisations");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    //CreateStatusBar();
    //SetStatusText("Welcome to wxWidgets!");

    // Bind events for transparency menu options
    Bind(wxEVT_MENU, &MyFrame::OnTransparencyMethod1, this, ID_TRANSPARENCY_METHOD1);
    Bind(wxEVT_MENU, &MyFrame::OnTransparencyMethod2, this, ID_TRANSPARENCY_METHOD2);
    Bind(wxEVT_MENU, &MyFrame::OnTransparencyMethod3, this, ID_TRANSPARENCY_METHOD3);

    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

void MyFrame::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);

    // Clear the background with black color
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawRectangle(0, 0, GetClientSize().x, GetClientSize().y);  // Fill entire window with black

    if (m_backgroundImage.IsOk())
    //if(1==0)
    {
        // Scale the image according to zoom level
        wxImage scaledImage = m_backgroundImage.ConvertToImage();
        scaledImage.Rescale(m_backgroundImage.GetWidth() * m_zoomLevel, m_backgroundImage.GetHeight() * m_zoomLevel);

        wxBitmap scaledBitmap(scaledImage);

        // Draw the scaled and panned image
        dc.DrawBitmap(scaledBitmap, m_panOffset.x, m_panOffset.y, false);
    }
    if (m_colourmap.IsOk())
    {
        // Scale the heat map overlay according to zoom level
        wxImage scaledHeatMap = m_colourmap.ConvertToImage();
        scaledHeatMap.Rescale(m_colourmap.GetWidth() * m_zoomLevel, m_colourmap.GetHeight() * m_zoomLevel);
        wxBitmap scaledHeatMapBitmap(scaledHeatMap);

        // Draw the scaled and panned heat map overlay with transparency enabled
        dc.DrawBitmap(scaledHeatMapBitmap, m_panOffset.x, m_panOffset.y, true);
    }

}

void MyFrame::OnMouseWheel(wxMouseEvent& event)
{
    int rotation = event.GetWheelRotation();
    double zoomFactor = 1.1;  // Change zoom factor here if needed

    if (rotation > 0)
        m_zoomLevel *= zoomFactor;
    else if (rotation < 0)
        m_zoomLevel /= zoomFactor;

    Refresh();  // Repaint to apply zoom
}

void MyFrame::OnMouseClick(wxMouseEvent& event)
{

    // Check if the background image is loaded
    if (!m_backgroundImage.IsOk())
    {
        wxLogError("Background image not loaded!");
        return;  // Return early if image is not loaded
    }

    // Get the mouse position relative to the window
    wxPoint mousePos = event.GetPosition();

    // Apply the pan offset to the mouse position (accounting for any panning)
    mousePos.x -= m_panOffset.x;
    mousePos.y -= m_panOffset.y;

    // Apply the zoom factor to adjust the coordinates based on the zoom level
    double scaledX = mousePos.x / m_zoomLevel;
    double scaledY = mousePos.y / m_zoomLevel;
}

void MyFrame::OnLeftMouseDown(wxMouseEvent& event)
{
    if (event.LeftIsDown())
    {
        // Start panning only if the left mouse button is clicked
        m_isPanning = true;
        m_lastMousePos = event.GetPosition();
        CaptureMouse();  // Capture mouse to track movement outside window bounds
    }

}


void MyFrame::OnRightMouseDown(wxMouseEvent& event)
{
    if (event.RightIsDown())
    {

        // Get the mouse position relative to the window
        wxPoint mousePos = event.GetPosition();

        // Apply the pan offset to the mouse position (accounting for any panning)
        mousePos.x -= m_panOffset.x;
        mousePos.y -= m_panOffset.y;

        // Apply the zoom factor to adjust the coordinates based on the zoom level
        double scaledX = mousePos.x / m_zoomLevel;
        double scaledY = mousePos.y / m_zoomLevel;

        // Output the coordinates
        PixelCoordinate MouseClick = { scaledX, scaledY };

        GPSCoordinate topLeft = mapBounds.topLeft;
        GPSCoordinate topRight = mapBounds.topRight;
        GPSCoordinate bottomLeft = mapBounds.bottomLeft;
        GPSCoordinate bottomRight = mapBounds.bottomRight;

        GPSCoordinate gps = pixelToGPS(MouseClick, 1448, 1340, topLeft, topRight, bottomLeft, bottomRight);
        double value_csl_min = interpolate_closest_three_points(m_inputData.input_data_csl_min, gps.latitude, gps.longitude);
        double value_csl_max = interpolate_closest_three_points(m_inputData.input_data_csl_max, gps.latitude, gps.longitude);
        double value_TT_max = interpolate_closest_three_points(m_inputData.input_data_TT_max, gps.latitude, gps.longitude);
        double value_TT_min = interpolate_closest_three_points(m_inputData.input_data_TT_min, gps.latitude, gps.longitude);
        // Output the coordinates
        wxLogMessage("Minutes to to CSL %.2f -  %.2f \n Minutes to TT %.2f -  %.2f \n Coordinates (%.2f, %.2f) ", value_csl_min, value_csl_max, value_TT_min, value_TT_max, gps.latitude, gps.longitude);
    }

}

void MyFrame::OnMouseMove(wxMouseEvent& event)
{
    if (m_isPanning)
    {
        wxPoint currentMousePos = event.GetPosition();
        wxPoint delta = currentMousePos - m_lastMousePos;

        m_panOffset += delta;  // Update the pan offset by delta
        m_lastMousePos = currentMousePos;

        Refresh();  // Repaint to apply pan
    }
}

void MyFrame::OnMouseUp(wxMouseEvent& event)
{
    if (m_isPanning)
    {
        m_isPanning = false;
        ReleaseMouse();
    }
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a good tool",
        "About Commute Optimiser", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello!");
}




#endif //HEADER_H
