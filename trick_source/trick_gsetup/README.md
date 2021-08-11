## Format for json config file
{
    ['name'] : 'Name of the application',
    ['landing'] : {
        ['version'] : 'version number',
        ['desc'] : 'description of this app'
    },
    'sections' : {
        ['section 1'] : {
            'options' : {
                ['option 1'] : {
                    'type' : [<dir><bool><flag><envvar><radio><string>],
                    ['desc'] : 'long description of the option'
                    ['label'] : 'label for the option'
                }
            }
    }
}