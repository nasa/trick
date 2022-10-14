# Information

<p align="center">
    <table>
        <tr>
            <th colspan=5>Model Information</th>
        </tr>
        <tr>
            <td>Rating</td>
            <td colspan=4 align=center>~level-4</td>
        </tr>
        <tr>
            <td>POC</td>
            <td>Gary Turner</td>
            <td>gary.w.turner@nasa.gov</td>
            <td>281.244.0668</td>
            <td>~antares</td>
        </tr>
    </table>
</p>

## Synopsis

Provides a comporehensive utility for executing MonteCarlo analyses.
The model functions by executing the simualtion twice:
-- on the first pass, the simulation creates input files containing
   the dispersed parameters
-- on the second pass, the simulation executes each of the generated
   input files; any run can be re-run by executing the particular input
   file generated in the first pass.

## Documentation

This model is independently documented in the docs directory.

## Verification

This model contains independent verification test cases in the verif directory.
