function change(t) {
    if (t.classList.contains("off")) {
        var deviceName = t.id.replace("_switch", "");
        $.ajax({
            url: '/setDevices',
            type: 'POST',
            data: {
                [deviceName]: 'true',
                'sessionID': localStorage.getItem("sessionID")
            },
            success: function(data) {
                if (data.response == "OK") {
                    $("#" + t.id).attr("class", "switch on position-relative rounded-3");
                    $("#" + t.id.replace("_switch", "_dot")).attr("class", "switch-dot on d-flex justify-content-center align-items-center position-absolute rounded-3");
                    $("#" + t.id.replace("_switch", "_status")).text("ON");
                }
                else if (data.response == "Login") {
                    $("#modal_title").text("Login Required to Access");
                    $("#modal_text").text("To proceed with controlling the system, please log in first. Your credentials are required to ensure secure access.");
                    var myModalEl = document.getElementById('alertModal');
                    var myModal = new bootstrap.Modal(myModalEl);
                    myModal.show();
                }
                else if (data.response == "Automat") {
                    $("#modal_title").text("Automatic Mode Active");
                    $("#modal_text").text("When the " + t.id.replace("_switch", "") + " is set to Automatic Mode, manual on/off control is disabled. To adjust the " + t.id.replace("_switch", "") + " manually, please switch to Manual Mode first.");
                    var myModalEl = document.getElementById('alertModal');
                    var myModal = new bootstrap.Modal(myModalEl);
                    myModal.show();
                }
                else if (data.response == "Schedule") {
                    $("#modal_title").text("Schedule Mode Active");
                    $("#modal_text").text("When the " + t.id.replace("_switch", "") + " is set to Schedule Mode, manual on/off control is disabled. To adjust the " + t.id.replace("_switch", "") + " manually, please switch to Manual Mode first.");
                    var myModalEl = document.getElementById('alertModal');
                    var myModal = new bootstrap.Modal(myModalEl);
                    myModal.show();
                }
            },
            error: function(xhr, status, error) {
                $("#modal_title").text("Connection Error");
                $("#modal_text").text("We're unable to connect to our servers. Please check your network settings and try again.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
        });
    }
    else if (t.classList.contains("on")) {
        var deviceName = t.id.replace("_switch", "");
        $.ajax({
            url: '/setDevices',
            type: 'POST',
            data: {
                [deviceName]: 'false',
                'sessionID': localStorage.getItem("sessionID")
            },
            success: function(data) {
                if (data.response == "OK") {
                    $("#" + t.id).attr("class", "switch off position-relative rounded-3");
                    $("#" + t.id.replace("_switch", "_dot")).attr("class", "switch-dot off d-flex justify-content-center align-items-center position-absolute rounded-3");
                    $("#" + t.id.replace("_switch", "_status")).text("OFF");
                    $("#" + t.id.replace("_switch", "_operation")).text("00:00:00");
                }
                else if (data.response == "Login") {
                    $("#modal_title").text("Login Required to Access");
                    $("#modal_text").text("To proceed with controlling the system, please log in first. Your credentials are required to ensure secure access.");
                    var myModalEl = document.getElementById('alertModal');
                    var myModal = new bootstrap.Modal(myModalEl);
                    myModal.show();
                }
                else if (data.response == "Automat") {
                    $("#modal_title").text("Automatic Mode Active");
                    $("#modal_text").text("When the " + t.id.replace("_switch", "") + " is set to Automatic Mode, manual on/off control is disabled. To adjust the " + t.id.replace("_switch", "") + " manually, please switch to Manual Mode first.");
                    var myModalEl = document.getElementById('alertModal');
                    var myModal = new bootstrap.Modal(myModalEl);
                    myModal.show();
                }
                else if (data.response == "Schedule") {
                    $("#modal_title").text("Schedule Mode Active");
                    $("#modal_text").text("When the " + t.id.replace("_switch", "") + " is set to Schedule Mode, manual on/off control is disabled. To adjust the " + t.id.replace("_switch", "") + " manually, please switch to Manual Mode first.");
                    var myModalEl = document.getElementById('alertModal');
                    var myModal = new bootstrap.Modal(myModalEl);
                    myModal.show();
                }
            },
            error: function(xhr, status, error) {
                $("#modal_title").text("Connection Error");
                $("#modal_text").text("We're unable to connect to our servers. Please check your network settings and try again.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
        });
    }
}

function loginBtn(t, loginPage = false) {
    let username = $('#username').val();
    let password = $('#password').val();
    $.ajax({
        url: '/login',
        type: 'POST',
        data: {
            'username': username,
            'password': password,
        },
        beforeSend: function() {
            $(t).html("Login <span class=\"spinner-border spinner-border-sm\"></span>");
        },
        complete: function() {
            if (!loginPage) {
                $(t).html("Login");
            }
        },
        success: function(data) {
            if (data.response == "OK") {
                localStorage.setItem("sessionID", data.sessionID);
                if (!loginPage) {
                    $("#username").val("");
                    $("#password").val("");
                    $('#loginModal').modal('hide');
                }
                else {
                    window.location.href = '/status';
                }
            }
            else if (data.response == "Invalid") {
                $("#username").val("");
                $("#password").val("");
                if (!loginPage) {
                    $("#alert_text").text("The username or password is invalid.");
                    $("#alert_text").attr("style", "line-height: 0.5;");
                }
                else {
                    $(t).html("Login");
                    $('#alertModal').modal('hide');
                    $("#modal_title").text("Login Failed");
                    $("#modal_text").text("The username or password is invalid.");
                    var myModalEl = document.getElementById('alertModal');
                    var myModal = new bootstrap.Modal(myModalEl);
                    myModal.show();
                }
                $("#username").focus();
            }
        },
        error: function(xhr, status, error) {
            $("#modal_title").text("Connection Error");
            $("#modal_text").text("We're unable to connect to our servers. Please check your network settings and try again.");
            var myModalEl = document.getElementById('alertModal');
            var myModal = new bootstrap.Modal(myModalEl);
            myModal.show();
        }
    });
}

function logoutBtn(t) {
    $.ajax({
        url: '/logout',
        type: 'POST',
        data: {
            'sessionID': localStorage.getItem("sessionID")
        },
        beforeSend: function() {
            t.innerHTML = "Yes <span class=\"spinner-border spinner-border-sm\"></span>";
        },
        complete: function() {
            if (!data.forceLogin) {
                t.innerHTML = "Yes";
            }
        },
        success: function(data) {
            if (data.response == "OK") {
                localStorage.removeItem("sessionID");
                if (!data.forceLogin) {
                    $('#logoutModal').modal('hide');
                }
            }
        },
        error: function(xhr, status, error) {
            $("#modal_title").text("Connection Error");
            $("#modal_text").text("We're unable to connect to our servers. Please check your network settings and try again.");
            var myModalEl = document.getElementById('alertModal');
            var myModal = new bootstrap.Modal(myModalEl);
            myModal.show();
        }
    });
}

function showPassword(t) {
    if (t.checked) {
        $("#password").attr("type", "text");
    } else {
        $("#password").attr("type", "password");
    }
}

function showAPPassword(t) {
    if (t.checked) {
        $("#apPass").attr("type", "text");
    } else {
        $("#apPass").attr("type", "password");
    }
}

function showSTPassword(t) {
    if (t.checked) {
        $("#stPass").attr("type", "text");
    } else {
        $("#stPass").attr("type", "password");
    }
}

function showACPassword(t) {
    if (t.checked) {
        $("#oldPassword").attr("type", "text");
        $("#newPassword").attr("type", "text");
    } 
    else {
        $("#oldPassword").attr("type", "password");
        $("#newPassword").attr("type", "password");
    }
}

function showHistory(t) {
    let year = $('#year').val();
    let month = $('#month').val();
    let value = $('#value').val();
    $.ajax({
        url: '/showHistory',
        type: 'POST',
        dataType: 'json',
        data: {
            'sessionID': localStorage.getItem("sessionID")
        },
        data: {
            'year': year,
            'month': month,
            'value': value
        },
        beforeSend: function() {
            t.innerHTML = "Show <span class=\"spinner-border spinner-border-sm\"></span>";
        },
        complete: function() {
            t.innerHTML = "Show";
        },
        success: function(data) {
            let table = "";
            if ($("#value").val() != "uptime") {
                table = `<table class='table table-bordered text-center'>
                    <thead>
                        <tr>
                            <th class='bg-purple'>Date</th>
                            <th class='bg-purple'>Minimum</th>
                            <th class='bg-purple'>Record at</th>
                            <th class='bg-purple'>Maximum</th>
                            <th class='bg-purple'>Record at</th>
                        </tr>
                    </thead>
                    <tbody>`;
            }
            else {
                table = `<table class='table table-bordered text-center'>
                    <thead>
                        <tr>
                            <th class='bg-purple'>Date</th>
                            <th class='bg-purple'>Heater</th>
                            <th class='bg-purple'>Cooler</th>
                            <th class='bg-purple'>Fogger</th>
                            <th class='bg-purple'>Exfan</th>
                            <th class='bg-purple'>Light</th>
                            <th class='bg-purple'>Pump</th>
                            <th class='bg-purple'>CO2 Gen</th>
                        </tr>
                    </thead>
                    <tbody>`;
            }
            table += data.table;
            table += "</tbody></table>";
            $('#showTable').html(table);
        },
        error: function(xhr, status, error) {
            $("#modal_title").text("Connection Error");
            $("#modal_text").text("We're unable to connect to our servers. Please check your network settings and try again.");
            var myModalEl = document.getElementById('alertModal');
            var myModal = new bootstrap.Modal(myModalEl);
            myModal.show();
        }
    });
}

function deleteAll(t) {
    $.ajax({
        url: "/deleteAll",
        type: 'POST',
        data: {
            'sessionID': localStorage.getItem("sessionID")
        },
        beforeSend: function() {
            t.innerHTML = "Yes <span class=\"spinner-border spinner-border-sm\"></span>";
        },
        complete: function() {
            t.innerHTML = "Yes";
        },
        success: function (data) {
            if (data.response == "OK") {
                $('#deleteModal').modal('hide');
                $("#modal_title").text("History Deleted Successfully");
                $("#modal_text").text("The history has been deleted successfully.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
            else if (data.response == "Login") {
                $('#deleteModal').modal('hide');
                $("#modal_title").text("Login Required to Access");
                $("#modal_text").text("To proceed with controlling the system, please log in first. Your credentials are required to ensure secure access.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
        },
        error: function (data) {
            $("#modal_title").text("Connection Error");
            $("#modal_text").text("We're unable to connect to our servers. Please check your network settings and try again.");
            var myModalEl = document.getElementById('alertModal');
            var myModal = new bootstrap.Modal(myModalEl);
            myModal.show();
        }
    });
}

function config(t) {
    var item = t.id;
    var value = t.value;
    $.ajax({
        url: '/setSettings',
        type: 'POST',
        data: {
            [item]: value
        },
        success: function(data) {
            if (data.response == "Login") {
                $("#timezone").prop("selectedIndex", zoneLast);
                $("#unit").prop("selectedIndex", unitLast);
                $("#calc").prop("selectedIndex", calcLast);
                $("#buzzer").prop("selectedIndex", buzzerLast);
                $("#delay").prop("selectedIndex", delayLast);
                $("#modal_title").text("Login Required to Access");
                $("#modal_text").text("To proceed with controlling the system, please log in first. Your credentials are required to ensure secure access.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
        },
        error: function(xhr, status, error) {
            $("#timezone").prop("selectedIndex", zoneLast);
            $("#unit").prop("selectedIndex", unitLast);
            $("#calc").prop("selectedIndex", calcLast);
            $("#buzzer").prop("selectedIndex", buzzerLast);
            $("#delay").prop("selectedIndex", delayLast);
            $("#modal_title").text("Connection Error");
            $("#modal_text").text("We're unable to connect to our servers. Please check your network settings and try again.");
            var myModalEl = document.getElementById('alertModal');
            var myModal = new bootstrap.Modal(myModalEl);
            myModal.show();
        }
    });
}

function changeMode(t) {
    let id = t.id;
    let val = $(t).val();
    let modes = {
        manual: "Man",
        automat: "Auto",
        schedule: "Sche",
        interval: "Inte"
    };

    $.each(modes, function (key, suffix) {
        $("#" + id + suffix).attr("style", val === key ? "" : "display: none;");
    });
}

function addSchedule(t) {
    t.innerHTML = "Add <span class=\"spinner-border spinner-border-sm\"></span>";
    $.ajax({
        url: '/setSchedule',
        type: 'POST',
        data: {
            'sessionID': localStorage.getItem("sessionID"),
            'day': $("#daySelect").val(),
            'device': device,
            'begin': Number($("#startHour").val()) * 3600 + Number($("#startMinute").val()) * 60 + Number($("#startSecond").val()),
            'end': Number($("#endHour").val()) * 3600 + Number($("#endMinute").val()) * 60 + Number($("#endSecond").val())
        },
        beforeSend: function() {
            t.innerHTML = "Add <span class=\"spinner-border spinner-border-sm\"></span>";
        },
        complete: function() {
            t.innerHTML = "Add";
        },
        success: function(data) {
            if (data.response == "OK") {
                $.ajax({
                    url: "/getSettings",
                    type: 'POST',
                    dataType: 'json',
                    data: {
                        'sessionID': localStorage.getItem("sessionID")
                    },
                    success: function (data) {
                        t.innerHTML = "Add";
                        $('#scheModal').modal('hide');
                        $('#' + device + 'List').html(data[device + 'List']);
                    },
                    error: function () {
                        $('#scheModal').modal('hide');
                        $("#modal_title").text("Connection Error");
                        $("#modal_text").text("We're unable to connect to our servers. Please check your network settings and try again.");
                        var myModalEl = document.getElementById('alertModal');
                        var myModal = new bootstrap.Modal(myModalEl);
                        myModal.show();
                    }
                });
            }
            else if (data.response == "Login") {
                $('#scheModal').modal('hide');
                $("#modal_title").text("Login Required to Access");
                $("#modal_text").text("To proceed with controlling the system, please log in first. Your credentials are required to ensure secure access.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
            else if (data.response == "Invalid") {
                $('#scheModal').modal('hide');
                $("#modal_title").text("Invalid Inputs");
                $("#modal_text").text("The input values ​​are invalid. Please check your inputs.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
        },
        error: function(xhr, status, error) {
            t.innerHTML = "Add";
            $("#modal_title").text("Connection Error");
            $("#modal_text").text("We're unable to connect to our servers. Please check your network settings and try again.");
            var myModalEl = document.getElementById('alertModal');
            var myModal = new bootstrap.Modal(myModalEl);
            myModal.show();
        }
    });
}

function removeSchedule(t) {
    t.innerHTML = "Delete Schedule <span class=\"spinner-border spinner-border-sm\"></span>";
    let items = $('#' + device + 'List').val();
    items.forEach(item => {
        const parts = item.trim().split(' ');
        const day = parts[0];
        const value = parts[1].replace("(", "") + "-" + parts[3].replace(")", "");
        $.ajax({
            url: '/removeSchedule',
            type: 'POST',
            data: {
                'sessionID': localStorage.getItem("sessionID"),
                'day': day,
                'device': device,
                'value': value
            },
            success: function(data) {
                if (data.response == "OK") {
                    $.ajax({
                        url: "/getSettings",
                        type: 'POST',
                        dataType: 'json',
                        success: function (data) {
                            t.innerHTML = "Delete Schedule";
                            $('#scheModal').modal('hide');
                            $('#' + device + 'List').html(data[device + 'List']);
                        },
                        error: function () {
                            $('#scheModal').modal('hide');
                            $("#modal_title").text("Connection Error");
                            $("#modal_text").text("We're unable to connect to our servers. Please check your network settings and try again.");
                            var myModalEl = document.getElementById('alertModal');
                            var myModal = new bootstrap.Modal(myModalEl);
                            myModal.show();
                        }
                    });
                }
                else if (data.response == "Login") {
                    $('#scheModal').modal('hide');
                    $("#modal_title").text("Login Required to Access");
                    $("#modal_text").text("To proceed with controlling the system, please log in first. Your credentials are required to ensure secure access.");
                    var myModalEl = document.getElementById('alertModal');
                    var myModal = new bootstrap.Modal(myModalEl);
                    myModal.show();
                }
            },
            error: function(xhr, status, error) {
                t.innerHTML = "Delete Schedule";
                $("#modal_title").text("Connection Error");
                $("#modal_text").text("We're unable to connect to our servers. Please check your network settings and try again.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
        });
    });
}

function saveSettings(t) {
    $.ajax({
        url: '/setSettings',
        type: 'POST',
        data: {
            'sessionID': localStorage.getItem("sessionID"),
            'hours' : $("#hours").val(),
            'minutes' : $("#minutes").val(),
            'seconds' : $("#seconds").val(),
            'year' : $("#year").val(),
            'month' : $("#month").val(),
            'day' : $("#day").val(),
            'unit' : $("#unit").val(),
            'calc' : $("#calc").val(),
            'buzzer' : $("#buzzer").val(),
            'forceLogin' : $("#force").val(),
            'delay' : $("#delay").val(),
            'error' : $("#error").val(),
            'ignore' : $("#ignore").val(),
            'tempOffset' : $("#tempOffset").val(),
            'rhOffset' : $("#rhOffset").val(),
            'heater' : $("#heater").val(),
            'heaterThresh' : $("#heaterThresh").val(),
            'cooler' : $("#cooler").val(),
            'coolerThresh' : $("#coolerThresh").val(),
            'fogger' : $("#fogger").val(),
            'foggerThresh' : $("#foggerThresh").val(),
            'exfan' : $("#exfan").val(),
            'exfanThresh' : $("#exfanThresh").val(),
            'light' : $("#light").val(),
            'lightThresh' : $("#lightThresh").val(),
            'pump' : $("#pump").val(),
            'pumpThresh' : $("#pumpThresh").val(),
            'co2gen' : $("#co2gen").val(),
            'co2genThresh' : $("#co2genThresh").val()
        },
        beforeSend: function() {
            t.innerHTML = "Save Changes <span class=\"spinner-border spinner-border-sm\"></span>";
        },
        complete: function() {
            t.innerHTML = "Save Changes";
        },
        success: function (data) {
            if (data.response == "OK") {
                $("#modal_title").text("Settings Saved Successfully");
                $("#modal_text").text("Your settings have been saved successfully.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
            else if (data.response == "Login") {
                $("#modal_title").text("Login Required to Access");
                $("#modal_text").text("To proceed with controlling the system, please log in first. Your credentials are required to ensure secure access.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
        },
        error: function (data) {
            $("#modal_title").text("Connection Error");
            $("#modal_text").text("We're unable to connect to our servers. Please check your network settings and try again.");
            var myModalEl = document.getElementById('alertModal');
            var myModal = new bootstrap.Modal(myModalEl);
            myModal.show();
        }
    });
}

function saveNetwork(t) {
    $.ajax({
        url: '/setNetwork',
        type: 'POST',
        data: {
            'sessionID': localStorage.getItem("sessionID"),
            'apName': $("#apName").val(),
            'apPass': $("#apPass").val(),
            'stName': $("#stName").val(),
            'stPass': $("#stPass").val(),
            'oldUsername': $("#oldUsername").val(),
            'newUsername': $("#newUsername").val(),
            'oldPassword': $("#oldPassword").val(),
            'newPassword': $("#newPassword").val(),
            'netMode': $("#netMode").val(),
            'ip': $("#ipAddress").val(),
            'subnet': $("#subnetMask").val(),
            'gateway': $("#gateway").val(),
            'dns': $("#dns").val()
        },
        beforeSend: function() {
            t.innerHTML = "Save Changes <span class=\"spinner-border spinner-border-sm\"></span>";
        },
        complete: function() {
            t.innerHTML = "Save Changes";
        },
        success: function (data) {
            if (data.response == "OK") {
                $("#modal_title").text("Settings Saved Successfully");
                $("#modal_text").text("Your settings have been saved successfully.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
            else if (data.response == "Login") {
                $("#modal_title").text("Login Required to Access");
                $("#modal_text").text("To proceed with controlling the system, please log in first. Your credentials are required to ensure secure access.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
            else if (data.response == "apNameNull") {
                $("#modal_title").text("Invalid AP Name");
                $("#modal_text").text("Access Point name cannot be empty.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
            else if (data.response == "apPassShort") {
                $("#modal_title").text("Weak Password");
                $("#modal_text").text("Access Point password must be at least 8 characters.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
            else if (data.response == "stPassShort") {
                $("#modal_title").text("Weak Password");
                $("#modal_text").text("Wi-Fi password must be at least 8 characters.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
            else if (data.response == "wrongPass") {
                $("#modal_title").text("Wrong Password");
                $("#modal_text").text("Password doesn't match. Please try again.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
            else if (data.response == "emptyPass") {
                $("#modal_title").text("Empty Password");
                $("#modal_text").text("New Password field cannot be empty.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
            else if (data.response == "shortPass") {
                $("#modal_title").text("Weak Password");
                $("#modal_text").text("Password must be at least 8 characters long.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
            else if (data.response == "InvalidIP") {
                $("#modal_title").text("Invalid IP");
                $("#modal_text").text("Enter a valid IP address.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
            
        },
        error: function (data) {
            $("#modal_title").text("Connection Error");
            $("#modal_text").text("We're unable to connect to our servers. Please check your network settings and try again.");
            var myModalEl = document.getElementById('alertModal');
            var myModal = new bootstrap.Modal(myModalEl);
            myModal.show();
        }
    });
}

function restoreSettings() {
    $("#unit").val("centigrade");
    $("#calc").val("gregorian");
    $("#buzzer").val("enable");
    $("#delay").val("10");
    $("#error").val("3600");
    $("#ignore").val("3");
    $("#tempOffset").val("0.0");
    $("#rhOffset").val("0.0");
    $("#heater").val("manual");
    $("#heater").trigger("change");
    $("#cooler").val("manual");
    $("#cooler").trigger("change");
    $("#fogger").val("manual");
    $("#fogger").trigger("change");
    $("#exfan").val("manual");
    $("#exfan").trigger("change");
    $("#light").val("manual");
    $("#light").trigger("change");
    $("#pump").val("manual");
    $("#pump").trigger("change");
    $("#co2gen").val("manual");
    $("#co2gen").trigger("change");
}

function resetModule(t) {
    $.ajax({
        url: "/restart",
        type: 'POST',
        data: {
            'sessionID': localStorage.getItem("sessionID")
        },
        beforeSend: function() {
            t.innerHTML = "Yes <span class=\"spinner-border spinner-border-sm\"></span>";
        },
        complete: function() {
            t.innerHTML = "Yes";
        },
        success: function (data) {
            if (data.response == "OK") {
                $('#resetModal').modal('hide');
                $("#modal_title").text("Module Restarted Successfully");
                $("#modal_text").text("The module has been restarted successfully.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
            else if (data.response == "Login") {
                $('#resetModal').modal('hide');
                $("#modal_title").text("Login Required to Access");
                $("#modal_text").text("To proceed with controlling the system, please log in first. Your credentials are required to ensure secure access.");
                var myModalEl = document.getElementById('alertModal');
                var myModal = new bootstrap.Modal(myModalEl);
                myModal.show();
            }
        },
        error: function (data) {
            $("#modal_title").text("Connection Error");
            $("#modal_text").text("We're unable to connect to our servers. Please check your network settings and try again.");
            var myModalEl = document.getElementById('alertModal');
            var myModal = new bootstrap.Modal(myModalEl);
            myModal.show();
        }
    });
}

function modeChange(t) {
    if ($(t).val() === "static") {
        $("#staticFields").removeClass("d-none");
    } else {
        $("#staticFields").addClass("d-none");
    }
}