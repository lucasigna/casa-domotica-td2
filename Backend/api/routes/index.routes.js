const express = require("express")
const router = express.Router()

router.get('*', (req, res) => {
    res.status(404).send({error: 404, description: 'Ruta no implementada'})
})

module.exports = router;