#!/bin/bash

# Function to process a text file and create directories
txtToMap() {
    local filename="$1"
    local modifier="${filename: -5:1}"

    SIZE_OF_MAP=10
    declare -A map

    # Function to find neighboring points
    # Function to find neighboring points
    findNeighboringPoints() {
        local x="$1"
        local y="$2"
        local neighbors=()

        # 5 stands for out of bounds
        neighbors[0]=$((x - 1 >= 0 ? map["$((x - 1)),$y"] : 5))
        neighbors[1]=$((x + 1 < SIZE_OF_MAP ? map["$((x + 1)),$y"] : 5))
        neighbors[2]=$((y - 1 >= 0 ? map["$x,$((y - 1))"] : 5))
        neighbors[3]=$((y + 1 < SIZE_OF_MAP ? map["$x,$((y + 1))"] : 5))

        echo "${neighbors[@]}"
    }


    # Function to create a text file with neighboring points
    createTxt() {
        local folderName="$1"
        local i="$2"
        local j="$3"

        local folderName2="$folderName/neighboring_points.txt"
        local neighboringPoints=($(findNeighboringPoints "$i" "$j"))

        echo "${neighboringPoints[@]}" > "$folderName2"
    }

    # Function to convert text file to map
    processTextFile() {
        local file="$1"
        local x=0
        local y=0

        while IFS= read -r line; do
            for ((i = 0; i < ${#line}; i++)); do
                if [[ "${line:i:1}" =~ [0-9] ]]; then
                    map["$x,$y"]="${line:i:1}"
                    ((y++))
                fi
            done
            ((x++))
            y=0
        done < "$file"

        # Print the map
        for ((i = 0; i < SIZE_OF_MAP; i++)); do
            for ((j = 0; j < SIZE_OF_MAP; j++)); do
                echo -n "${map["$i,$j"]} "
            done
            echo
        done
    }

    # Main loop
    for ((i = 0; i < SIZE_OF_MAP; i++)); do
        for ((j = 0; j < SIZE_OF_MAP; j++)); do
            folderName="/home/garrett_masuda9/Final-Project/final/World/${modifier}${i}${j}"
            mkdir -p "$folderName"
            createTxt "$folderName" "$i" "$j"
        done
    done

    # Process the text file
    processTextFile "$filename"
}

# Function to scan directories and process text files
scanDirectory() {
    local path="$1"

    for entry in "$path"/*; do
        if [ -d "$entry" ]; then
            scanDirectory "$entry"
        elif [[ "$entry" == *.txt ]]; then
            echo "Found text file: $entry"
            txtToMap "$entry"
        fi
    done
}

# Main function
main() {
    local folderPath="./Map"
    scanDirectory "$folderPath"
}

main