const validationMiddleware = (req,res,next) => {
    if(process.env.ADMIN == 'true'){
        // Usuario validado
        next()
    } else {
        // Usuario sin permisos
        res.json({error: "Usuario sin permisos para esta acción"})
    }
}

module.exports = validationMiddleware;